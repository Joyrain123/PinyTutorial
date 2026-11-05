#pragma once
#include "DJIMotor/DJIMotor.hpp"
#include "DMMotor.hpp"

namespace MOTOR {

class Motor {
public:
    void ctrl(float _current);

    void init();
private:
    DJIMotor djimotor_;
    DMMotor dmMotor_;
};

} // namespace MOTOR