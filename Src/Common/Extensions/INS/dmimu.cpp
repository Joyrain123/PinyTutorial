#include "dmimu.hpp"
#include <cstring>

namespace INS {

float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    const uint16_t raw = _bytes[0] | (static_cast<uint16_t>(_bytes[1]) << 8);
    return (static_cast<float>(raw) / 65535.0f * _range) + _offset;
}

bool Ins::init(canHandle *_hcan)
{
    if (_hcan == nullptr || hcan_ != nullptr) {
        return false;
    }
    const auto result = Can::instance().registerCallback(
            _hcan, MASTERID, [this](const uint8_t *_frame) { receiveFromISR(_frame); }, 8);
    if (result != HAL_OK) {
        return false;
    }
    hcan_ = _hcan;
    lastRestartMs_ = HAL_GetTick() - RESTART_INTERVAL_MS;
    return true;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    if (_frame[0] != 3) {
        return;
    }
    // 中断只保存最新欧拉角帧，解析和发送命令放到任务中。
    std::memcpy(rxFrame_, _frame, 8);
    rxTimeMs_ = HAL_GetTick();
    rxPending_ = true;
}

void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    if (!hasData_ || std::memcmp(_frame + 2, rawEuler_, 6) != 0
            || (_timeMs - lastRxMs_) > OFFLINE_TIMEOUT_MS) {
        lastChangeMs_ = _timeMs;
    }
    std::memcpy(rawEuler_, _frame + 2, 6);
    data_.pitch = phrase(_frame + 2, 180.0f, -90.0f);
    data_.yaw = phrase(_frame + 4, 360.0f, -180.0f);
    data_.roll = phrase(_frame + 6, 360.0f, -180.0f);
    lastRxMs_ = _timeMs;
    hasData_ = true;
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    uint8_t request[8] = { 0xCC, _rid, static_cast<uint8_t>(_write), 0xDD, 0, 0, 0, 0 };
    return Can::instance().transmitData(hcan_, IMUID, request, 8) == HAL_OK;
}

void Ins::update()
{
    if (hcan_ == nullptr) {
        return;
    }
    taskENTER_CRITICAL();
    if (rxPending_) {
        processFrame(rxFrame_, rxTimeMs_);
        rxPending_ = false;
    }
    taskEXIT_CRITICAL();

    const uint32_t now = HAL_GetTick();
    // 持续收到的欧拉角 5 秒不变时重启，两次尝试至少间隔 5 秒。
    if (isOnline() && static_cast<uint32_t>(now - lastChangeMs_) >= EULER_STUCK_MS
            && static_cast<uint32_t>(now - lastRestartMs_) >= RESTART_INTERVAL_MS) {
        lastRestartMs_ = now;
        if (transmit(0, true)) {
            taskENTER_CRITICAL();
            hasData_ = false;
            rxPending_ = false;
            taskEXIT_CRITICAL();
        }
        return;
    }
    // 只补查缺失的欧拉角，不查询加速度和陀螺仪。
    if ((!hasData_ || static_cast<uint32_t>(now - lastRxMs_) >= EULER_POLL_MS)
            && static_cast<uint32_t>(now - lastRequestMs_) >= EULER_POLL_MS) {
        lastRequestMs_ = now;
        transmit(3, false);
    }
}

void Ins::task()
{
    constexpr TickType_t PERIOD = configTICK_RATE_HZ / RX_FREQ > 0 ? configTICK_RATE_HZ / RX_FREQ : 1;
    TickType_t lastWake = xTaskGetTickCount();
    while (true) {
        update();
        vTaskDelayUntil(&lastWake, PERIOD);
    }
}

Data_s Ins::getData() const
{
    taskENTER_CRITICAL();
    const Data_s copy = data_;
    taskEXIT_CRITICAL();
    return copy;
}

bool Ins::isOnline() const
{
    taskENTER_CRITICAL();
    const bool online = hasData_ && static_cast<uint32_t>(HAL_GetTick() - lastRxMs_) <= OFFLINE_TIMEOUT_MS;
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    const uint32_t now = HAL_GetTick();
    const bool ready = hasData_ && static_cast<uint32_t>(now - lastRxMs_) <= OFFLINE_TIMEOUT_MS
            && static_cast<uint32_t>(now - lastChangeMs_) < EULER_STUCK_MS;
    taskEXIT_CRITICAL();
    return ready;
}

} // namespace INS
