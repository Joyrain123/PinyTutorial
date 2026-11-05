#include "dmimu.hpp"
#include <cstring>

namespace INS {

float Ins::phrase(const uint8_t *_bytes, float _range, float _offset)
{
    // TODO:
    return 0.0f;
}

bool Ins::init(canHandle *_hcan)
{
    // TODO:
    return false;
}

void Ins::receiveFromISR(const uint8_t *_frame)
{
    // TODO:
}

void Ins::processFrame(const uint8_t *_frame, uint32_t _timeMs)
{
    // TODO:
}

bool Ins::transmit(uint8_t _rid, bool _write)
{
    // TODO:
    return false;
}

void Ins::update()
{
    taskENTER_CRITICAL();
    //这里需要尽量少的放东西，处理重要数据放在这里，
    //其作用可以暂时理解为当程序走到这里的时候，不完成里面的程序就不会退出去走下面的程序
    // TODO:
    taskEXIT_CRITICAL();

    // TODO:
}

void Ins::task()
{
    constexpr TickType_t PERIOD = configTICK_RATE_HZ / RX_FREQ > 0 ? configTICK_RATE_HZ / RX_FREQ : 1;
    TickType_t lastWake = xTaskGetTickCount();
    while (true) {
        //这里可以暂时理解为裸机开发，也就是平时使用的whlie循环里的东西
        update();
        vTaskDelayUntil(&lastWake, PERIOD);
    }
}

Data_s Ins::getData() const
{
    taskENTER_CRITICAL();
    // TODO:
    const Data_s copy = {};
    taskEXIT_CRITICAL();
    return copy;
}

//这里的Online和ready并不冲突，至于为什么，实践的时候会遇到这个问题的
bool Ins::isOnline() const
{
    taskENTER_CRITICAL();
    // TODO:
    const bool online = false;
    taskEXIT_CRITICAL();
    return online;
}

bool Ins::isReady() const
{
    taskENTER_CRITICAL();
    // TODO:
    const bool ready = false;
    taskEXIT_CRITICAL();
    return ready;
}

} // namespace INS
