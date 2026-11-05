#pragma once

#include <cstdint>
#include "Bsp_can.hpp"

namespace MOTOR {

struct DJIRegInfo_s {
    canHandle *hcan;
    uint8_t id;
    uint8_t txSlot; // 0~6
};

} // namespace MOTOR
