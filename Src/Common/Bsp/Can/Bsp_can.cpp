#include "Bsp_can.hpp"
#include "Soc.hpp"

extern canHandle HCAN1;
extern canHandle HCAN2;
#if SOC_CAN_NUM == 3
extern canHandle HCAN3;
#endif

HAL_StatusTypeDef Can::init() {}

HAL_StatusTypeDef Can::registerCallback(canHandle *_hcan, uint32_t _stdid, callback _pCallback)
{
    struct Handler_s temp = { .stdid = _stdid, .func = _pCallback };
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
#if SOC_CAN_NUM == 3
    } else if (_hcan == &HCAN3) {
        if (can3cnt < MAX_RECV_DEVICE) {
            cbTable3[can3cnt] = temp;
            can3cnt++;
            return HAL_OK;
        } else {
            return HAL_ERROR;
        }
#endif
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
#if SOC_CAN_NUM == 3
    } else if (_hcan == &HCAN3) {
        for (uint8_t i = 0; i < can3cnt; i++) {
            if (cbTable3[i].stdid == _stdid) {
                cbTable3[i].func = nullptr;
                break;
            }
        }
#endif
    }
}

#if defined(SOC_FDCAN)

#define DLC(n) FDCAN_DLC_BYTES_##n

struct DlcTable_s {
    uint8_t dlc;
    uint32_t bytes;
};

const DlcTable_s dlc[] = {
    { .dlc = 0, .bytes = DLC(0) },   { .dlc = 1, .bytes = DLC(1) },   { .dlc = 2, .bytes = DLC(2) },
    { .dlc = 3, .bytes = DLC(3) },   { .dlc = 4, .bytes = DLC(4) },   { .dlc = 5, .bytes = DLC(5) },
    { .dlc = 6, .bytes = DLC(6) },   { .dlc = 7, .bytes = DLC(7) },   { .dlc = 8, .bytes = DLC(8) },
    { .dlc = 12, .bytes = DLC(12) }, { .dlc = 16, .bytes = DLC(16) }, { .dlc = 20, .bytes = DLC(20) },
    { .dlc = 24, .bytes = DLC(24) }, { .dlc = 32, .bytes = DLC(32) }, { .dlc = 48, .bytes = DLC(48) },
    { .dlc = 64, .bytes = DLC(64) },
};

HAL_StatusTypeDef fdcanFilterInit(FDCAN_HandleTypeDef *_hcan, uint8_t _fifo) {}

HAL_StatusTypeDef Can::initSelf(canHandle *_hcan, uint32_t _fifo) {}

HAL_StatusTypeDef Can::transmitData(canHandle *_hcan, uint16_t _stdid, uint8_t *_tx_data, uint32_t _len) {}

HAL_StatusTypeDef Can::transmitBrsData(canHandle *_hcan, uint16_t _stdid, uint8_t *_tx_data, uint32_t _len) {}

void Can::checkBus(canHandle *_hfdcan)
{
    FDCAN_ProtocolStatusTypeDef protocolStatus = {};

    HAL_FDCAN_GetProtocolStatus(_hfdcan, &protocolStatus);
    if (protocolStatus.BusOff) {
        CLEAR_BIT(_hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);
        /* Restart CAN */
        // HAL_FDCAN_Stop(_hfdcan);
        // HAL_FDCAN_Start(_hfdcan);
    }
}

inline void Can::callbackFromISR(canHandle *_hcan, uint32_t _rxFifo)
{
    uint8_t rxData[8];
    canHeader rxHeader;
    if (HAL_FDCAN_GetRxMessage(_hcan, _rxFifo, &rxHeader, rxData) != HAL_OK) {
        return;
    }

    if (_hcan == &HCAN1) {
        for (uint8_t i = 0; i < can1cnt; i++) {
            if (cbTable1[i].stdid == rxHeader.Identifier) {
                cbTable1[i].func(rxData);
                break;
            }
        }
    } else if (_hcan == &HCAN2) {
        for (uint8_t i = 0; i < can2cnt; i++) {
            if (cbTable2[i].stdid == rxHeader.Identifier) {
                cbTable2[i].func(rxData);
                break;
            }
        }
    } else if (_hcan == &HCAN3) {
        for (uint8_t i = 0; i < can3cnt; i++) {
            if (cbTable3[i].stdid == rxHeader.Identifier) {
                cbTable3[i].func(rxData);
                break;
            }
        }
    }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *_hfdcan, uint32_t _rxFifo0ITs)
{
    UNUSED(_rxFifo0ITs);
    Can::instance().callbackFromISR(_hfdcan, RX_FIFO0);
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *_hfdcan, uint32_t _rxFifo1ITs)
{
    UNUSED(_rxFifo1ITs);
    Can::instance().callbackFromISR(_hfdcan, RX_FIFO1);
}

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *_hfdcan, uint32_t _errorStatusITs)
{
    if ((_errorStatusITs & FDCAN_IT_BUS_OFF) != RESET) {
        Can::instance().checkBus(_hfdcan);
    }
}

#elif defined(SOC_CAN)
HAL_StatusTypeDef canFilterInit(CAN_HandleTypeDef *_can, uint32_t _fifo) {}

HAL_StatusTypeDef Can::initSelf(canHandle *_hcan, uint32_t _fifo) {}

HAL_StatusTypeDef Can::transmitData(canHandle *_hcan, uint16_t _stdid, uint8_t *_txData, uint32_t _len) {}

void Can::callbackFromISR(canHandle *_hcan, uint32_t _rxFifo)
{
    uint8_t rxData[8];
    canHeader rxHeader;
    if (HAL_CAN_GetRxMessage(_hcan, _rxFifo, &rxHeader, rxData) != HAL_OK) {
        return;
    }
    if (_hcan == &HCAN1) {
        for (uint8_t i = 0; i < can1cnt; i++) {
            if (cbTable1[i].stdid == rxHeader.StdId) {
                cbTable1[i].func(rxData);
                break;
            }
        }
    } else if (_hcan == &HCAN2) {
        for (uint8_t i = 0; i < can2cnt; i++) {
            if (cbTable2[i].stdid == rxHeader.StdId) {
                cbTable2[i].func(rxData);
                break;
            }
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *_hcan) { Can::instance().callbackFromISR(_hcan, RX_FIFO0); }

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *_hcan) { Can::instance().callbackFromISR(_hcan, RX_FIFO1); }

#endif
