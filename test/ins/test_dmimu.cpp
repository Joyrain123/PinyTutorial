#include "mock_platform.hpp"
#include <iostream>
#define private public
#include "dmimu.hpp"
#undef private

static void require(bool condition, const char *message)
{
    if (!condition) throw std::runtime_error(message);
}

static std::array<uint8_t, 8> euler(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f)
{
    std::array<uint8_t, 8> bytes{3,25,0,0,0,0,0,0};
    const float values[] = {pitch,yaw,roll};
    for (unsigned i = 0; i < 3; ++i) {
        const float span = i == 0 ? 180.0f : 360.0f;
        const auto raw = static_cast<uint16_t>((values[i] + span / 2.0f) / span * 65535.0f);
        bytes[2+2*i] = raw & 0xff;
        bytes[3+2*i] = raw >> 8;
    }
    return bytes;
}

static void run(INS::Ins &imu, uint32_t begin, uint32_t end, bool changing = false, uint32_t origin = 0)
{
    for (uint32_t t = begin; t < end; ++t) {
        mockTick = origin + t;
        const auto bytes = euler(changing ? static_cast<float>(t % 300) * 0.05f : 0.0f);
        Can::instance().callback(bytes.data());
        imu.update();
    }
}

static unsigned restartCount()
{
    unsigned count = 0;
    for (const auto &sent : Can::instance().sent) {
        require(sent.id == 1 && sent.bytes[0] == 0xCC && sent.bytes[3] == 0xDD,
                "Unexpected CAN command format");
        if (sent.bytes[2] == 1) {
            require(sent.bytes == std::array<uint8_t,8>{0xCC,0,1,0xDD,0,0,0,0},
                    "Restart must use the documented command");
            ++count;
        } else require(sent.bytes[1] == 3,"Only Euler data may be requested");
    }
    return count;
}

int main(int argc, char **argv)
{
    try {
        require(argc == 2,"Expected scenario name");
        const std::string scenario = argv[1];
        const uint32_t origin = scenario == "rollover" ? UINT32_MAX - 2500u : 0;
        mockTick = origin;
        auto &imu = INS::Ins::instance();
        canHandle handle;
        require(!imu.init(nullptr),"Null CAN handle must be rejected");
        require(imu.init(&handle),"Initialization failed");
        require(!imu.init(&handle),"Duplicate initialization must be rejected");

        if (scenario == "decode") {
            const auto bytes = euler(17.0f,-35.0f,66.0f);
            Can::instance().callback(bytes.data());
            imu.update();
            const auto angles = imu.getData();
            require(imu.isReady() && std::abs(angles.pitch - 17.0f) < 0.01f
                    && std::abs(angles.yaw + 35.0f) < 0.01f && std::abs(angles.roll - 66.0f) < 0.01f,
                    "Euler-only data must become ready and preserve pitch/yaw/roll mapping");
            for (uint8_t rid : {1,2,4}) {
                auto other = bytes;
                other[0] = rid;
                other[2] ^= 0x7f;
                Can::instance().callback(other.data());
                imu.update();
            }
            require(imu.getData().pitch == angles.pitch,"Other frame types must be ignored");
        } else if (scenario == "changing") {
            run(imu,0,16000,true);
            require(imu.isReady() && restartCount() == 0,"Changing Euler angles must not restart");
        } else if (scenario == "frozen" || scenario == "rollover") {
            run(imu,0,5000,false,origin);
            require(imu.isReady() && restartCount() == 0,"Must wait the full five seconds");
            mockTick = origin + 5000;
            require(imu.isOnline() && !imu.isReady(),"Frozen data must expire before the task next runs");
            run(imu,5000,5001,false,origin);
            require(restartCount() == 1 && !imu.isReady(),"Frozen Euler data must send one restart");
            run(imu,5001,10001,false,origin);
            require(restartCount() == 1,"The next frozen-data window must not restart early");
            run(imu,10001,10002,false,origin);
            require(restartCount() == 2,"Persistent freezing may retry after another five seconds");
            run(imu,10002,11000,true,origin);
            require(imu.isReady() && restartCount() == 2,"Changing data must recover after a restart");
        } else if (scenario == "offline") {
            run(imu,0,1);
            for (uint32_t t = 1; t < 6000; ++t) { mockTick=t; imu.update(); }
            require(!imu.isOnline() && !imu.isReady() && restartCount() == 0,
                    "No received data must not be mistaken for continuously frozen data");
            run(imu,6000,6001);
            require(imu.isReady() && restartCount() == 0,"Reconnect must start a new observation window");
        } else if (scenario == "temperature") {
            for (uint32_t t = 0; t < 5001; ++t) {
                mockTick=t;
                auto bytes = euler();
                bytes[1] = static_cast<uint8_t>(t);
                Can::instance().callback(bytes.data());
                imu.update();
            }
            require(restartCount() == 1,"Temperature changes must not hide unchanged Euler angles");
        } else if (scenario == "tx_failure") {
            Can::instance().status = HAL_BUSY;
            run(imu,0,5001);
            require(restartCount() == 1 && !imu.isReady(),"A failed restart must leave frozen data invalid");
            run(imu,5001,10000);
            require(restartCount() == 1,"Failed commands must also respect the five-second retry interval");
            run(imu,10000,10001);
            require(restartCount() == 2,"Failed restart must remain retryable");
        } else if (scenario == "request_mode") {
            auto &can = Can::instance();
            can.replies[2] = euler(17.0f);
            can.answerReads = true;
            for (uint32_t t = 0; t < 4000; ++t) { mockTick=t; imu.update(); }
            require(imu.isReady() && std::abs(imu.getData().pitch - 17.0f) < 0.01f
                    && can.sent.size() < 850 && restartCount() == 0,
                    "Fallback requests must obtain only Euler data at the limited rate");
        } else throw std::runtime_error("Unknown scenario");
        std::cout << "PASS " << scenario << '\n';
    } catch (const std::exception &error) {
        std::cerr << "FAIL " << error.what() << '\n';
        return 1;
    }
}
