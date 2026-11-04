#pragma once

#include "Bsp_can.hpp"
#include "Singleton.hpp"
#include "Task.hpp"
#include <cstdint>

namespace INS {

struct Data_s {
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;
};

class Ins : public Singleton<Ins>, public Task<Ins, 256> {
public:
    static constexpr uint16_t MASTERID = 0x11;
    static constexpr uint16_t IMUID = 0x01;
    static constexpr uint16_t RX_FREQ = 1000;

    bool init(canHandle *_hcan);
    Data_s getData() const;
    bool isOnline() const;
    bool isReady() const;
    void task();

private:
    Ins() : Task("INS", TaskPriority_e::HIGH1) {}
    friend class Singleton<Ins>;

    void receiveFromISR(const uint8_t *_frame);
    void update();
    void processFrame(const uint8_t *_frame, uint32_t _timeMs);
    bool transmit(uint8_t _rid, bool _write);
    float phrase(const uint8_t *_bytes, float _range, float _offset);

    static constexpr uint32_t OFFLINE_TIMEOUT_MS = 100;
    static constexpr uint32_t EULER_POLL_MS = 5;
    static constexpr uint32_t EULER_STUCK_MS = 5000; // 欧拉角不变多久后重启，可按测试需要修改。
    static constexpr uint32_t RESTART_INTERVAL_MS = 5000;

    canHandle *hcan_ = nullptr;
    Data_s data_{};
    uint8_t rawEuler_[6]{};
    uint8_t rxFrame_[8]{};
    uint32_t rxTimeMs_ = 0;
    volatile bool rxPending_ = false;
    bool hasData_ = false;
    uint32_t lastRxMs_ = 0;
    uint32_t lastChangeMs_ = 0;
    uint32_t lastRequestMs_ = 0;
    uint32_t lastRestartMs_ = 0;
};

} // namespace INS