#include "App.hpp"
#include "DJIMotor.hpp"
#include "Soc.hpp"
#include "MotorManager.hpp"
#include "FreeRTOS.h"
#include "main.h"
#include <algorithm>

#if CAN_ASSESSMENT_ADVANCED
static INS::Ins *imuInstance = nullptr;
#endif

void App::init()
{
    motor.init();
#if CAN_ASSESSMENT_ADVANCED
    imuInstance = &INS::Ins::instance();
    imuInstance->init(&HCAN2);
#endif
}

void App::task()
{
#if CAN_ASSESSMENT_BASIC
    motor.ctrl(CAN_BASIC_CURRENT_MA / 1000.0f);
#else
    constexpr float MAX_CURRENT = 1.0f;
    constexpr float MAX_CURRENT_STEP = 0.01f;

    if (imuInstance == nullptr || !imuInstance->isReady()) {
        angle_ = {};
        motorCurrent_ = 0.0f;
        motor.ctrl(0.0f);
        return;
    }
    angle_ = imuInstance->getData();
    // TODO:
    const float targetCurrent = std::clamp(0.0f, -MAX_CURRENT, MAX_CURRENT);
    motorCurrent_ = std::clamp(targetCurrent, motorCurrent_ - MAX_CURRENT_STEP,
                               motorCurrent_ + MAX_CURRENT_STEP);

    motor.ctrl(motorCurrent_);
#endif
}