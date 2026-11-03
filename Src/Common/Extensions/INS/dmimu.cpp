#include "dmimu.hpp"
#include "projdefs.h"

namespace INS {

float Ins::phrase(const uint8_t *_bytes, float _range, float _offsetid)
{
    const uint16_t raw = _bytes[0] | (static_cast<uint16_t>(_bytes[1]) << 8);
    return (static_cast<float>(raw) / 65535.0f * _range) + _offsetid;
}

bool Ins::init(canHandle *_hcan)
{
    rxQueue_ = xQueueCreateStatic(RX_QUEUE_LENGTH, sizeof(RxFrame_s), rxQueueStorage_, &rxQueueControl_);
    if (rxQueue_ == nullptr) {
        return false;
    }

    hcan_ = _hcan;

    const auto result = Can::instance().registerCallback(
            hcan_, MASTERID, [this](const uint8_t *_frame) { receiveFromISR(_frame); }, 8);
    return (result == HAL_OK);
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    if (_frame[0] != 3) {
        return;
    }

    RxFrame_s received;
    for (uint8_t i = 0; i < 8; ++i) {
        received.data[i] = _frame[i];
    }
    received.timeMs = HAL_GetTick();
    BaseType_t pxHigherPriorityTaskWoken = 0;
    if (xQueueSendFromISR(rxQueue_, &received, &pxHigherPriorityTaskWoken) != pdPASS) {
        droppedFrames_ = droppedFrames_ + 1;
    }
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

void Ins::update()
{
    RxFrame_s frame;

    for (uint8_t i = 0; i < RX_QUEUE_LENGTH; ++i) {
        if (xQueueReceive(rxQueue_, &frame, 0) != pdTRUE) {
            break;
        }

        Data_s next;
        next.pitch = phrase(frame.data + 2, 180.0f, -90.0f);
        next.yaw = phrase(frame.data + 4, 360.0f, -180.0f);
        next.roll = phrase(frame.data + 6, 360.0f, -180.0f);

        taskENTER_CRITICAL();
        data_ = next;
        lastRxMs_ = frame.timeMs;
        hasData_ = true;
        taskEXIT_CRITICAL();
    }
}

void Ins::requestEuler()
{
    uint8_t data[8] = { 0xCC, 0x03, 0x00, 0xDD, 0, 0, 0, 0 };
    Can::instance().transmitData(hcan_, IMUID, data, 8);
}

void Ins::task()
{
    constexpr TickType_t PERIOD = configTICK_RATE_HZ / RX_FREQ;
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
            update();
            requestEuler();
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
    const bool received = hasData_;
    const uint32_t lastRx = lastRxMs_;
    taskEXIT_CRITICAL();

    return received && static_cast<uint32_t>(HAL_GetTick() - lastRx) <= OFFLINE_TIMEOUT_MS;
}

} // namespace INS
