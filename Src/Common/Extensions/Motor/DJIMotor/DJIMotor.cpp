#include "DJIMotor.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

using namespace MOTOR;

void DJIMotor::init(canHandle *_hcan, uint8_t _id)
{
    regInfo_.hcan = _hcan;
    regInfo_.id = _id;
    // TODO:
}

bool DJIMotor::cmdCurrent(float _current)
{
    if (HAL_GetTick() - lastTxMs_ < 1)
        return false;

    lastTxMs_ = HAL_GetTick();
    uint8_t tx[8] = { 0 };
    uint16_t code = currentToCode(_current);
    // TODO:
    return false;
}

uint16_t DJIMotor::currentToCode(float _current)
{
    constexpr float NUM_PER_AMP = CURRENT_CODE_MAX / CURRENT_MAX;
    float code = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX) * NUM_PER_AMP;
    int32_t v = static_cast<int32_t>(std::lround(code));
    v = v > 16384 ? 16384 : (v < -16384 ? -16384 : v);
    return static_cast<uint16_t>(static_cast<int16_t>(v));
}