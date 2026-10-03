#include "MotorManager.hpp"
#include "DJIMotor.hpp"
#include "Soc.hpp"

using namespace MOTOR;

extern canHandle HCAN1;

void Motor::init() {
    djimotor_.init(&hcan1, 2);
}

void Motor::ctrl() {
    // DJI
    djimotor_.update();
    djimotor_.cmdCurrent(0.5f);

    // DM
    dmMotor_.update();
    dmMotor_.cmdVdes(1.f);
}