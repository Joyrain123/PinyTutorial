#pragma once

#include "RegMsgs.hpp"
#include <cstdint>
#include "Bsp_can.hpp"

namespace MOTOR {

struct DJIRegInfo_s {
    canHandle *hcan;
    uint8_t id;
    uint8_t txSlot; // 0~6
    volatile uint8_t rxWriteId_ = 0;
    volatile uint8_t rxReadId_ = 0;
};

struct DMRegInfo_s {
    canHandle *hcan;
    uint8_t rxBaseId = 0x10;
    uint8_t txBaseId = 0;
    uint8_t offsetId;
    RegId_e regId;
    uint8_t dat[4] = {};
    bool isReverse;
};

struct DJIFeedback_s {
    uint16_t rawAng;
    int16_t rawRpm;
    int16_t current;
    uint8_t temperature;
};

#pragma pack(push, 1)
struct DMFeedback_s {
    uint8_t ID : 4;
    ErrorCode_e errorCode : 4;
    uint16_t rawAng : 14;
    uint16_t rawVel : 12;
    uint16_t torque : 12;
    uint8_t mosTemperature : 8;
    uint8_t rotorTemperature : 8;
};

struct MITMsg_s {
    uint16_t exptScale : 16;
    uint16_t exptVel : 12;
    uint16_t Kp : 12;
    uint16_t Kd : 12;
    uint16_t torqueForward : 12;
};

struct EMITMsg_s {
    float exptScale;
    uint16_t exptVelX100 : 16;
    uint16_t imaxX10000 : 16;
};
struct PDESVDESMsg_s {
    float exptScale;
    float exptVel;
};

struct VDESMsg_s {
    float exptVel;
    float reserved;
};
#pragma pack(pop)

struct Data_s {
    // 原始编码器角度(rad)
    float rawAng;
    // 软件设置零点(rad)
    float zeroAng;
    // 相对零点角度(rad)
    float ang;
    // 上一次角度(rad)
    float angLast;
    // 单圈角度(rad)
    float singleCirAng;
    // 多圈累计角度(rad)
    float multipCirAng;
    // 圈数
    float cirNum;
    // 角速度 rad/s
    float spdRadps;
    // 转速 rpm
    float spdRpm;
    // 电流 A
    float curr;
    // 扭矩 Nm
    float torq;
    // 温度 ℃
    float temperature;
};

} // namespace MOTOR
