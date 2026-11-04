#pragma once
#ifndef DM_IMU_HOST_TEST
#error "IMU mocks may only be used by the host test target"
#endif
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

using TickType_t = uint32_t;
using BaseType_t = int;
constexpr int pdFALSE = 0, pdTRUE = 1, pdPASS = 1;
constexpr uint32_t configTICK_RATE_HZ = 1000;
#define taskENTER_CRITICAL() do {} while (false)
#define taskEXIT_CRITICAL() do {} while (false)
#define portYIELD_FROM_ISR(value) do { (void)(value); } while (false)
inline uint32_t mockTick = 0;
inline uint32_t HAL_GetTick() { return mockTick; }
inline TickType_t xTaskGetTickCount() { return mockTick; }
inline void vTaskDelayUntil(TickType_t *, TickType_t) {}

template<class T> class Singleton {
public:
    static T &instance() { static T value; return value; }
};
enum class TaskPriority_e { HIGH1 };
template<class T, unsigned N> class Task {
public:
    Task(const char *, TaskPriority_e) {}
};

struct canHandle {};
enum HAL_StatusTypeDef { HAL_OK, HAL_ERROR, HAL_BUSY };
class Can {
public:
    struct Sent { uint16_t id; std::array<uint8_t, 8> bytes; };
    static Can &instance() { static Can value; return value; }
    std::function<void(const uint8_t *)> callback;
    std::vector<Sent> sent;
    HAL_StatusTypeDef status = HAL_OK;
    bool answerReads = false;
    std::array<std::array<uint8_t, 8>, 3> replies{};
    HAL_StatusTypeDef registerCallback(canHandle *, uint32_t id,
                                      std::function<void(const uint8_t *)> fn, uint8_t length)
    {
        if (id != 0x11 || length != 8) throw std::runtime_error("Invalid CAN subscription");
        callback = fn;
        return HAL_OK;
    }
    HAL_StatusTypeDef transmitData(canHandle *, uint16_t id, uint8_t *bytes, uint32_t length)
    {
        if (length != 8) throw std::runtime_error("Invalid CAN frame length");
        Sent item{id,{}};
        std::copy(bytes, bytes + 8, item.bytes.begin());
        sent.push_back(item);
        if (status == HAL_OK && answerReads && bytes[2] == 0 && bytes[1] >= 1 && bytes[1] <= 3)
            callback(replies[bytes[1] - 1].data());
        return status;
    }
};

struct StaticQueue_t {
    uint8_t *storage = nullptr;
    unsigned length = 0, size = 0, head = 0, tail = 0, count = 0;
};
using QueueHandle_t = StaticQueue_t *;
inline QueueHandle_t xQueueCreateStatic(unsigned length, unsigned size, uint8_t *storage, StaticQueue_t *queue)
{
    *queue = {storage,length,size,0,0,0};
    return queue;
}
inline BaseType_t xQueueSendFromISR(QueueHandle_t queue, const void *data, BaseType_t *)
{
    if (queue->count == queue->length) return pdFALSE;
    std::memcpy(queue->storage + queue->tail * queue->size, data, queue->size);
    queue->tail = (queue->tail + 1) % queue->length;
    ++queue->count;
    return pdPASS;
}
inline BaseType_t xQueueReceive(QueueHandle_t queue, void *data, TickType_t)
{
    if (queue->count == 0) return pdFALSE;
    std::memcpy(data, queue->storage + queue->head * queue->size, queue->size);
    queue->head = (queue->head + 1) % queue->length;
    --queue->count;
    return pdTRUE;
}
inline void xQueueReset(QueueHandle_t queue) { queue->head = queue->tail = queue->count = 0; }
