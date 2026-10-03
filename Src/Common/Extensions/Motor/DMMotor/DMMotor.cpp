#include "DMMotor.hpp"
#include "RegMsgs.hpp"
#include "Model.hpp"
#include "MotorUtils.hpp"
#include <cstdint>
#include <cstring>

using namespace MOTOR;

void DMMotor::init(canHandle *_hcan, uint8_t _id)
{
    regInfo_.hcan = _hcan;
    regInfo_.offsetId = _id;
    memcpy(&status_, &dm3507, sizeof(dm3507)); // 确定电机型号
    Can::instance().registerCallback(regInfo_.hcan, regInfo_.txBaseId + regInfo_.offsetId,
                                     [this](const uint8_t *_data) {
                                         for (uint8_t i = 0; i < 8; i++) {
                                             rxBuf_[i] = _data[i];
                                         }
                                         parseReady_ = true;
                                     });
}

bool DMMotor::update()
{
    if (parseReady_) {
        parseReady_ = false;
        uint8_t raw[8];
        for (uint8_t i = 0; i < 8; i++)
            raw[i] = rxBuf_[i];
        parseFeedback(raw);
        lastRxMs_ = HAL_GetTick();
    }
    return true;
}

uint8_t DMMotor::send(uint16_t _sendId, uint8_t *_txBuf, uint8_t _len)
{
    if (HAL_GetTick() - lastTxMs_ < 1)
        return 0;
    lastTxMs_ = HAL_GetTick();
    return static_cast<uint8_t>(Can::instance().transmitData(regInfo_.hcan, _sendId, _txBuf, _len));
}

void DMMotor::writeReg(RegId_e _regId, uint8_t _dat[4])
{
    uint16_t id = regInfo_.offsetId;
    uint8_t writeTxBuffer[8] = { static_cast<uint8_t>(id),
                                 static_cast<uint8_t>(id >> 8),
                                 0x55,
                                 static_cast<uint8_t>(_regId),
                                 _dat[0],
                                 _dat[1],
                                 _dat[2],
                                 _dat[3] };
    send(0x7FF, writeTxBuffer, 8);
}

void DMMotor::switchCtrlMode(WorkMode_e _newMode)
{
    workMode_ = _newMode;
    regInfo_.regId = RegId_e::DM_REG_CTRL_MODE;
    switch (_newMode) {
    case WorkMode_e::MIT_TT:
    case WorkMode_e::MIT_VDESPDES:
    case WorkMode_e::MIT_VDES: {
        ctrlId_ = regInfo_.offsetId;
        regInfo_.dat[0] = 0x01;
        writeReg(regInfo_.regId, regInfo_.dat);
        break;
    }
    case WorkMode_e::PDESVDES: {
        ctrlId_ = regInfo_.offsetId + 0x100;
        regInfo_.dat[0] = 0x02;
        writeReg(regInfo_.regId, regInfo_.dat);
        break;
    }
    case WorkMode_e::VDES: {
        ctrlId_ = regInfo_.offsetId + 0x200;
        regInfo_.dat[0] = 0x03;
        writeReg(regInfo_.regId, regInfo_.dat);
        break;
    }
    case WorkMode_e::EMIT: {
        ctrlId_ = regInfo_.offsetId + 0x300;
        regInfo_.dat[0] = 0x04;
        writeReg(regInfo_.regId, regInfo_.dat);
        break;
    }
    default:
        ctrlId_ = 0xFFFF;
        break;
    }
}

void DMMotor::parseFeedback(const uint8_t *_rxBuf)
{
    DMFeedback_s fb = {};
    fb.ID = _rxBuf[0] & 0x0F;
    fb.errorCode = static_cast<ErrorCode_e>(_rxBuf[0] >> 4);
    fb.rawAng = (_rxBuf[1] << 8) | _rxBuf[2];
    fb.rawVel = (_rxBuf[3] << 4) | (_rxBuf[4] >> 4);
    fb.torque = ((_rxBuf[4] & 0xF) << 8 | _rxBuf[5]);
    fb.mosTemperature = _rxBuf[6];
    fb.rotorTemperature = _rxBuf[7];

    errorCode_ = fb.errorCode;

    float noumenaAng = static_cast<float>(fb.rawAng) / MEASURE_MAX * 2.f * PI;
    data_.rawAng = regInfo_.isReverse ? (2.f * PI) - noumenaAng : noumenaAng;
    float del = data_.rawAng - data_.zeroAng;
    data_.ang = del < 0 ? del + (2.f * PI) : del;

    float noumenaVel = uint2float(fb.rawVel, -status_.VMax, status_.VMax, 12);
    data_.spdRadps = regInfo_.isReverse ? -noumenaVel : noumenaVel;
    data_.spdRpm = radps2rpm(data_.spdRadps);

    float noumenaTorq = uint2float(fb.torque, -status_.TMax, status_.TMax, 12);
    data_.torq = regInfo_.isReverse ? -noumenaTorq : noumenaTorq;
    data_.curr = data_.torq / status_.Kn;

    data_.temperature = fb.mosTemperature;

    data_.singleCirAng = rangeMap(data_.ang);
    data_.cirNum = data_.ang / (2.f * PI);

    data_.multipCirAng = data_.singleCirAng + (TWO_PI * data_.cirNum);
    data_.angLast = data_.ang;
}
