#pragma once
#include "DJIMotor/DJIMotor.hpp"

namespace MOTOR {

class Motor {
public:
    void ctrl(float _current);

    void init();
private:
    DJIMotor djimotor_;
};

} // namespace MOTOR