#pragma once

#include "MotorMsgs.hpp"
#include "Bsp_can.hpp"

namespace MOTOR {

class DJIMotor {
public:
    //param
    
    static constexpr float CURRENT_MAX = 3.0f;
    static constexpr float CURRENT_CODE_MAX = 16384.0f;

    DJIMotor() = default;

    void init(canHandle *_hcan, uint8_t _id);

    bool cmdCurrent(float _txData);

private:
    // TODO:
    static constexpr uint32_t TX_STDID_1_4 = 0;
    // TODO:
    static constexpr uint32_t TX_STDID_5_7 = 0;

    uint16_t currentToCode(float _current);
    uint32_t txStdID();

    uint32_t lastTxMs_ = 0;
    DJIRegInfo_s regInfo_ = {};
};

} // namespace MOTOR