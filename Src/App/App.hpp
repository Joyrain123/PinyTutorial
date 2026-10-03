#pragma once
#include "Singleton.hpp"
#include "MotorManager.hpp"

class App : public Singleton<App> {
    // pwm呼吸灯、转舵机，串口，电机pid测试，遥控器上层实现，考核框架
public:
    void init();

    void task();

private:
    MOTOR::Motor motor;
};