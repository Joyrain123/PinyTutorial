#include "MotorManager.hpp"
#include "DJIMotor.hpp"
#include "Soc.hpp"

using namespace MOTOR;

extern canHandle HCAN1;

void Motor::init() {
    djimotor_.init(&hcan1, 2);
}

void Motor::ctrl(float _current) {
    djimotor_.cmdCurrent(_current);

}