#pragma once

#include "MotorMsgs.hpp"
#include "Bsp_can.hpp"

namespace MOTOR {

class DJIMotor {
public:
    static constexpr float PI = std::numbers::pi_v<float>;
    static constexpr float ANGLE_MAX = 8191.0f;
    static constexpr float CURRENT_MAX = 3.0f;
    static constexpr float CURRENT_CODE_MAX = 16384.0f;
    static constexpr float REDUCTION_RATIO = 1.0f;

    DJIMotor() = default;

    void init(canHandle *_hcan, uint8_t _id);

    bool update();

    bool cmdCurrent(float _txData);

    float rpm() const { return data_.spdRpm; };

    float angle() const { return data_.ang; }

private:
    static constexpr uint32_t RX_STDID = 0x204; //baseId
    static constexpr uint32_t TX_STDID_1_4 = 0x1FE;
    static constexpr uint32_t TX_STDID_5_7 = 0x2FE;
    static constexpr uint16_t MEASURE_MAX = 8191;

    void parseFeedback(const uint8_t *_raw);
    uint16_t currentToCode(float _current);
    uint32_t txStdID();

    volatile uint8_t rxBuf_[2][8];

    volatile bool frameReady_ = false;
    volatile uint16_t rxCnt_ = 0; //max:65535

    uint32_t lastRxMs_ = 0; // disconnection detection
    uint32_t lastTxMs_ = 0;
    Data_s data_ = {};
    DJIRegInfo_s regInfo_ = {};
};

} // namespace MOTOR