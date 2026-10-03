#include "DJIMotor.hpp"
#include <algorithm>
#include <cstdint>

using namespace MOTOR;

void DJIMotor::init(canHandle *_hcan, uint8_t _id)
{
    regInfo_.hcan = _hcan;
    regInfo_.id = _id;
    if (_id >= 1 && _id <= 4)
        regInfo_.txSlot = _id - 1;
    else if (_id >= 5 && _id <= 7)
        regInfo_.txSlot = _id - 5;
    Can::instance().registerCallback(_hcan, RX_STDID + _id, [this](const uint8_t *_data) {
        for (uint8_t i = 0; i < 8; i++)
            rxBuf_[regInfo_.rxWriteId_][i] = _data[i];
        frameReady_ = true;
        regInfo_.rxReadId_ = regInfo_.rxWriteId_;
        regInfo_.rxWriteId_ ^= 1;
    });
}

bool DJIMotor::update()
{
    if (frameReady_) {
        frameReady_ = false;
        uint8_t raw[8];
        for (uint8_t i = 0; i < 8; i++)
            raw[i] = rxBuf_[regInfo_.rxReadId_][i];
        parseFeedback(raw);
        lastRxMs_ = HAL_GetTick();
    }
    return true;
}

void DJIMotor::parseFeedback(const uint8_t *_raw)
{
    DJIFeedback_s fb;
    fb.rawAng = (uint16_t)((_raw[0] << 8) | _raw[1]);                // 原始角度编码 0~8191
    fb.rawRpm = (int16_t)((_raw[2] << 8) | _raw[3]);                 // 原始转速 rpm
    fb.current = (int16_t)((_raw[4] << 8) | _raw[5]);                // 原始电流编码
    fb.temperature = _raw[6];                                        // 温度 ℃
    float angle = (float)fb.rawAng / ANGLE_MAX * 2.0f * PI;          // 编码器角度转换为 rad
    data_.rawAng = angle;                                            // 原始角度(rad)
    data_.angLast = data_.ang;                                       // 保存上一周期角度
    data_.ang = angle - data_.zeroAng;                               // 零点修正后的角度
    data_.singleCirAng = data_.ang;                                  // 单圈角度
    data_.cirNum = data_.ang / (2.0f * PI);                          // 计算转过的圈数
    data_.multipCirAng = data_.cirNum * 2.0f * PI;                   // 多圈累计角度
    data_.spdRpm = (float)fb.rawRpm;                                 // 转速 rpm
    data_.spdRadps = data_.spdRpm * 2.0f * PI / 60.0f;               // 转速转换为 rad/s
    data_.curr = (float)fb.current / CURRENT_CODE_MAX * CURRENT_MAX; // 电流 A
    constexpr float KT = 0.741f;
    data_.torq = data_.curr * KT;              // 根据电流估算扭矩 Nm
    data_.temperature = (float)fb.temperature; // 电机温度 ℃
}

bool DJIMotor::cmdCurrent(float _current)
{
    if (HAL_GetTick() - lastTxMs_ < 1)
        return false;

    lastTxMs_ = HAL_GetTick();
    uint8_t tx[8] = { 0 };
    uint16_t code = currentToCode(_current);
    uint8_t index = regInfo_.txSlot * 2;
    tx[index] = (uint8_t)((code >> 8) & 0xFF);
    tx[index + 1] = (uint8_t)(code & 0xFF);
    uint32_t stdid;
    if (regInfo_.id <= 4) {
        stdid = TX_STDID_1_4;
    } else {
        stdid = TX_STDID_5_7;
    }
    return Can::instance().transmitData(regInfo_.hcan, stdid, tx, 8) == HAL_OK;
}

uint16_t DJIMotor::currentToCode(float _current)
{
    _current = std::clamp(_current, -CURRENT_MAX, CURRENT_MAX);
    return static_cast<uint16_t>(_current);
}