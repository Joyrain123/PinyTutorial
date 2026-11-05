#include "Bsp_can.hpp"
#include "Soc.hpp"
#include "stm32f4xx_hal_can.h"

extern canHandle HCAN1;
extern canHandle HCAN2;
#if SOC_CAN_NUM == 3
extern canHandle HCAN3;
#endif

HAL_StatusTypeDef Can::init()
{
    HAL_StatusTypeDef result = HAL_OK;
    result = initSelf(&HCAN1, RX_FIFO0);
    result = initSelf(&HCAN2, RX_FIFO1);
#ifdef HCAN3
    extern canHandle HCAN3;
    result = initSelf(&HCAN3, RX_FIFO0);
#endif
    return result;
}


HAL_StatusTypeDef Can::registerCallback(canHandle *_hcan, uint32_t _stdid, callback _pCallback,
                                       uint8_t _expectedLength)
{
    if (_expectedLength > 8 || !_pCallback) {
        return HAL_ERROR;
    }
    struct Handler_s temp = { .stdid = _stdid, .func = _pCallback, .expectedLength = _expectedLength };
    if (_hcan == &HCAN1) {
        if (can1cnt < MAX_RECV_DEVICE) {
            cbTable1[can1cnt] = temp;
            can1cnt++;
            return HAL_OK;
        } else {
            return HAL_ERROR;
        }
    } else if (_hcan == &HCAN2) {
        if (can2cnt < MAX_RECV_DEVICE) {
            cbTable2[can2cnt] = temp;
            can2cnt++;
            return HAL_OK;
        } else {
            return HAL_ERROR;
        }
    }
    return HAL_ERROR;
}

void Can::unregisterCallback(canHandle *_hcan, uint32_t _stdid)
{
    if (_hcan == &HCAN1) {
        for (uint8_t i = 0; i < can1cnt; i++) {
            if (cbTable1[i].stdid == _stdid) {
                cbTable1[i].func = nullptr;
                break;
            }
        }
    } else if (_hcan == &HCAN2) {
        for (uint8_t i = 0; i < can2cnt; i++) {
            if (cbTable2[i].stdid == _stdid) {
                cbTable2[i].func = nullptr;
                break;
            }
        }
    }
}

#if defined(SOC_CAN)
HAL_StatusTypeDef canFilterInit(CAN_HandleTypeDef *_can, uint32_t _fifo)
{
    HAL_StatusTypeDef result = HAL_OK;
    CAN_FilterTypeDef canFilter{};
    // TODO:
    result = HAL_CAN_ConfigFilter(_can, &canFilter);
    if (_fifo == 0) {
        result = HAL_CAN_ActivateNotification(_can, CAN_IT_RX_FIFO0_MSG_PENDING);
    } else if (_fifo == 1) {
        result = HAL_CAN_ActivateNotification(_can, CAN_IT_RX_FIFO1_MSG_PENDING);
    }
    return result;
}

HAL_StatusTypeDef Can::initSelf(canHandle *_hcan, uint32_t _fifo)
{
    HAL_StatusTypeDef result = HAL_OK;
    result = canFilterInit(_hcan, _fifo);
    __HAL_CAN_ENABLE_IT(_hcan, CAN_IT_BUSOFF);
    result = HAL_CAN_Start(_hcan);
    return result;
}

HAL_StatusTypeDef Can::transmitData(canHandle *_hcan, uint16_t _stdid, uint8_t *_txData, uint32_t _len)
{
    CAN_TxHeaderTypeDef txHeader;
    uint32_t canMailbox;
    txHeader.DLC = _len;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.StdId = _stdid;
    txHeader.TransmitGlobalTime = DISABLE;
    return HAL_CAN_AddTxMessage(_hcan, &txHeader, _txData, &canMailbox);
}

void Can::callbackFromISR(canHandle *_hcan, uint32_t _rxFifo)
{
    uint8_t rxData[8];
    canHeader rxHeader;
    if (HAL_CAN_GetRxMessage(_hcan, _rxFifo, &rxHeader, rxData) != HAL_OK) {
        return;
    }
    if (rxHeader.IDE != CAN_ID_STD || rxHeader.RTR != CAN_RTR_DATA) {
        return;
    }
    if (_hcan == &HCAN1) {
        for (uint8_t i = 0; i < can1cnt; i++) {
            if (cbTable1[i].stdid == rxHeader.StdId) {
                const auto &handler = cbTable1[i];
                if (handler.func && (handler.expectedLength == 0 || rxHeader.DLC == handler.expectedLength)) {
                    handler.func(rxData);
                }
                break;
            }
        }
    } else if (_hcan == &HCAN2) {
        for (uint8_t i = 0; i < can2cnt; i++) {
            if (cbTable2[i].stdid == rxHeader.StdId) {
                const auto &handler = cbTable2[i];
                if (handler.func && (handler.expectedLength == 0 || rxHeader.DLC == handler.expectedLength)) {
                    handler.func(rxData);
                }
                break;
            }
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *_hcan) { Can::instance().callbackFromISR(_hcan, RX_FIFO0); }

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *_hcan) { Can::instance().callbackFromISR(_hcan, RX_FIFO1); }

#endif
