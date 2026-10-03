#include "DMMotor.hpp"
#include "MotorMsgs.hpp"
#include "MotorUtils.hpp"
#include <algorithm>
#include <cstring>

using namespace MOTOR;

void DMMotor::serializeMITMsg(MITMsg_s &_msgMIT)
{
    txBuf_[0] = static_cast<uint8_t>((_msgMIT.exptScale & 0xFF00) >> 8);
    txBuf_[1] = static_cast<uint8_t>(_msgMIT.exptScale & 0x00FF);
    txBuf_[2] = static_cast<uint8_t>((_msgMIT.exptVel & 0xFF0) >> 4);
    txBuf_[3] = static_cast<uint8_t>((_msgMIT.exptVel & 0x00F) << 4 | ((_msgMIT.Kp & 0xF00) >> 8));
    txBuf_[4] = static_cast<uint8_t>(_msgMIT.Kp & 0x0FF);
    txBuf_[5] = static_cast<uint8_t>((_msgMIT.Kd & 0xFF0) >> 4);
    txBuf_[6] = static_cast<uint8_t>((_msgMIT.Kd & 0x00F) << 4 | ((_msgMIT.torqueForward & 0xF00) >> 8));
    txBuf_[7] = static_cast<uint8_t>(_msgMIT.torqueForward & 0x0FF);
}

void DMMotor::cmdMitTorq(float _torq)
{
    switchCtrlMode(WorkMode_e::MIT_TT);
    MITMsg_s msgMIT = {};
    msgMIT.Kp = msgMIT.Kd = 0;

    _torq = std::clamp(regInfo_.isReverse ? -_torq : _torq, -status_.TMax, status_.TMax);
    msgMIT.torqueForward = float2uint(_torq, -status_.TMax, status_.TMax, 12);

    serializeMITMsg(msgMIT);
    send(ctrlId_, txBuf_.data(), 8);
}

void DMMotor::cmdMitVdes(float _vel, float _kd, float _torq)
{
    switchCtrlMode(WorkMode_e::MIT_VDES);
    MITMsg_s msgMIT = {};
    msgMIT.Kd = float2uint(_kd, 0, status_.MITKdMax, 12);
    msgMIT.Kp = 0;

    _vel = std::clamp(regInfo_.isReverse ? -_vel : _vel, -status_.VMax, status_.VMax);
    msgMIT.exptVel = float2uint(_vel, -status_.VMax, status_.VMax, 12);

    _torq = std::clamp(regInfo_.isReverse ? -_torq : _torq, -status_.TMax, status_.TMax);
    msgMIT.torqueForward = float2uint(_torq, -status_.TMax, status_.TMax, 12);

    serializeMITMsg(msgMIT);
    send(ctrlId_, txBuf_.data(), 8);
}

void DMMotor::cmdMitPdesVdes(float _pos, float _vel, float _torq, float _kd, float _kp)
{
    switchCtrlMode(WorkMode_e::MIT_VDESPDES);
    MITMsg_s msgMIT = {};
    msgMIT.Kd = float2uint(_kd, 0, status_.MITKdMax, 12);
    msgMIT.Kp = float2uint(_kp, 0, status_.MITKpMax, 12);

    _pos = std::clamp(regInfo_.isReverse ? -_pos : _pos, -status_.PMax, status_.PMax);
    msgMIT.exptScale = float2uint(_pos, -status_.PMax, status_.PMax, 16);
    _vel = std::clamp(regInfo_.isReverse ? -_vel : _vel, -status_.VMax, status_.VMax);
    msgMIT.exptVel = float2uint(_vel, -status_.VMax, status_.VMax, 12);
    _torq = std::clamp(regInfo_.isReverse ? -_torq : _torq, -status_.TMax, status_.TMax);
    msgMIT.torqueForward = float2uint(_torq, -status_.TMax, status_.TMax, 12);

    serializeMITMsg(msgMIT);
    send(ctrlId_, txBuf_.data(), 8);
}

void DMMotor::cmdPdesVdes(float _pos, float _vel)
{
    switchCtrlMode(WorkMode_e::PDESVDES);
    PDESVDESMsg_s msgPDESVDES = {};

    _pos = regInfo_.isReverse ? -_pos : _pos;
    msgPDESVDES.exptScale = _pos;
    _vel = regInfo_.isReverse ? -_vel : _vel;
    msgPDESVDES.exptVel = _vel;

    memcpy(txBuf_.begin(), &msgPDESVDES.exptScale, 4);
    memcpy(&txBuf_[4], &msgPDESVDES.exptVel, 4);
    send(ctrlId_, txBuf_.data(), 8);
}

void DMMotor::cmdVdes(float _vel)
{
    switchCtrlMode(WorkMode_e::VDES);
    VDESMsg_s msgVDES = {};

    _vel = regInfo_.isReverse ? -_vel : _vel;
    msgVDES.exptVel = _vel;

    memcpy(txBuf_.begin(), &msgVDES.exptVel, 4);
    send(ctrlId_, txBuf_.data(), 4);
}

void DMMotor::cmdPVT(float _pos, float _vel, float _torq)
{
    switchCtrlMode(WorkMode_e::EMIT);
    EMITMsg_s msgEMIT = {};

    _pos = regInfo_.isReverse ? -_pos : _pos;
    msgEMIT.exptScale = _pos;
    _vel = regInfo_.isReverse ? -_vel : _vel;
    msgEMIT.exptVelX100 = static_cast<uint16_t>(((_vel < 0) ? -_vel : _vel) * 100.f);
    _torq = regInfo_.isReverse ? -_torq : _torq;
    msgEMIT.imaxX10000 = static_cast<uint16_t>(((_torq < 0) ? -_torq : _torq) /
                                               status_.Kn / status_.currMax * status_.currTxCodeSpan);

    memcpy(txBuf_.begin(), &msgEMIT, 8);
    send(ctrlId_, txBuf_.data(), 4);
}

