#include "App.hpp"
#include "DJIMotor.hpp"
#include "Soc.hpp"
#include "MotorManager.hpp"
#include "FreeRTOS.h"
#include "main.h"
#include <algorithm>

static INS::Ins *imuInstance = nullptr;

void App::init()
{
    motor.init();
    imuInstance = &INS::Ins::instance();
    imuInstance->init(&HCAN2);
}

void App::task()
{
    constexpr float MAX_CURRENT = 1.0f;
    constexpr float MAX_CURRENT_STEP = 0.01f;

    if (imuInstance == nullptr || !imuInstance->isReady()) {
        angle_ = {};
        motorCurrent_ = 0.0f;
        motor.ctrl(0.0f);
        return;
    }
    angle_ = imuInstance->getData();
    const float targetCurrent = std::clamp(angle_.pitch / 90.0f, -MAX_CURRENT, MAX_CURRENT);
    motorCurrent_ = std::clamp(targetCurrent, motorCurrent_ - MAX_CURRENT_STEP,
                               motorCurrent_ + MAX_CURRENT_STEP);

    motor.ctrl(motorCurrent_);
}