#pragma once

#include "Bsp_can.hpp"
#include "Singleton.hpp"
#include "../../Common/Kernel/Task.hpp"
#include "queue.h"
#include <memory>

namespace INS {

struct Data_s {
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;
};

class Ins : public Singleton<Ins>, public Task<Ins, 256> {
public:
    static constexpr uint16_t MASTERID = 0x11;
    static constexpr uint16_t IMUID = 0x01;    
    static constexpr uint16_t RX_FREQ = 1000; 

    bool init(canHandle *_hcan);

    Data_s getData() const;
    bool isOnline() const; 
    uint32_t getDroppedFrames();

    void task(); 

private:
    Ins() : Task("INS", TaskPriority_e::HIGH2) {}
    friend class Singleton<Ins>;

    void receiveFromISR(const uint8_t *_frame);
    void update();
    void requestEuler();
    float phrase(const uint8_t *_bytes, float _range, float _offset);

    static constexpr uint8_t RX_QUEUE_LENGTH = 8;
    static constexpr uint32_t OFFLINE_TIMEOUT_MS = 100;

    struct RxFrame_s {
        uint8_t data[8];
        uint32_t timeMs; 
    };

    canHandle *hcan_ = nullptr;

    QueueHandle_t rxQueue_ = nullptr;
    StaticQueue_t rxQueueControl_{};
    uint8_t rxQueueStorage_[RX_QUEUE_LENGTH * sizeof(RxFrame_s)]{};
    volatile uint32_t droppedFrames_ = 0;

    Data_s data_{};
    uint32_t lastRxMs_ = 0;
    bool hasData_ = false;
};

} // namespace INS