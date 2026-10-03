#pragma once

#include "MotorMsgs.hpp"
#include "RegMsgs.hpp"
#include "Bsp_can.hpp"
#include <cstdint>

namespace MOTOR {

struct Status_s {
    float PMax;
    float VMax;
    float TMax;
    float MITKpMax;
    float MITKdMax;
    float currTxCodeSpan;
    float currMax; // A
    float torqMax; // Nm
    float Kn;      // Nm/A
};

enum class WorkMode_e : uint8_t { UNKNOWN, MIT_TT, MIT_VDES, MIT_VDESPDES, PDESVDES, VDES, EMIT };

class DMMotor {
    using TxBuf = std::array<uint8_t, 8>;
public:
    static constexpr float PI = std::numbers::pi_v<float>;
    

    DMMotor() = default;

    void init(canHandle *_hcan, uint8_t _id);
    bool update();
    uint8_t send(uint16_t _sendId, uint8_t *_txBuf, uint8_t _len);

    void switchCtrlMode(WorkMode_e _newMode);

    float rpm() const { return data_.spdRpm; };
    float angle() const { return data_.ang; };

    void parseFeedback(const uint8_t *_rxBuf);
    void cmdMitVdes(float _vel, float _kd, float _torq);
    void cmdMitPdesVdes(float _pos, float _vel, float _torq, float _kd, float _kp);
    void cmdMitTorq(float _torq);
    void cmdPdesVdes(float _pos, float _vel);
    void cmdVdes(float _vel);
    void cmdPVT(float _pos, float _vel, float _torq);

    void writeReg(RegId_e _regId, uint8_t _dat[4]);


private:
    static constexpr uint16_t MEASURE_MAX = 16383;

    void serializeMITMsg(MITMsg_s &_msgMIT);

    WorkMode_e workMode_;
    uint16_t ctrlId_ = 0XFFFF;
    TxBuf txBuf_;
    Data_s data_ = {};
    ErrorCode_e errorCode_;
    DMRegInfo_s regInfo_;
    volatile uint8_t rxBuf_[8];
    bool parseReady_ = false;

    uint32_t lastRxMs_ = 0; // disconnection detection
    uint32_t lastTxMs_ = 0;
    Status_s status_;
};

} // namespace MOTOR