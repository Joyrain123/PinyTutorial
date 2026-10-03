#pragma once

namespace MOTOR {

struct DM3507_s {
    static constexpr float P_MAX = 12.57f;
    static constexpr float V_MAX = 15.71f; // 150rpm
    static constexpr float T_MAX = 3.0f;
    static constexpr float MIT_KP_MAX = 500.f;
    static constexpr float MIT_KD_MAX = 5.f;
    static constexpr float CURR_TX_CODE_SPAN = 10000.f;

    static constexpr float CURR_RATED = 3.f;
    static constexpr float TORQ_RATED = 0.8f;
    static constexpr float CURR_MAX = 8.3f;
    static constexpr float TORQ_MAX = T_MAX;
    static constexpr float KN = TORQ_RATED / CURR_RATED;
} dm3507;

struct DM4310_s {
    static constexpr float P_MAX = 12.57f;
    static constexpr float V_MAX = 12.57f; // 120rpm
    static constexpr float T_MAX = 7.0f;
    static constexpr float MIT_KP_MAX = 500.f;
    static constexpr float MIT_KD_MAX = 5.f;
    static constexpr float CURR_TX_CODE_SPAN = 10000.f;

    static constexpr float CURR_RATED = 3.7f;
    static constexpr float TORQ_RATED = 3.f;
    static constexpr float CURR_MAX = 7.2f;
    static constexpr float TORQ_MAX = T_MAX;
    static constexpr float KN = TORQ_RATED / CURR_RATED;
} dm4310;

struct DM4340_s {
    static constexpr float P_MAX = 12.57f;
    static constexpr float V_MAX = 3.77f; // 36rpm
    static constexpr float T_MAX = 27.f;
    static constexpr float MIT_KP_MAX = 500.f;
    static constexpr float MIT_KD_MAX = 5.f;
    static constexpr float CURR_TX_CODE_SPAN = 10000.f;

    static constexpr float CURR_RATED = 3.f;
    static constexpr float TORQ_RATED = 9.f;
    static constexpr float CURR_MAX = 8.f;
    static constexpr float TORQ_MAX = T_MAX;
    static constexpr float KN = TORQ_RATED / CURR_RATED;
} dm4340;

} // namespace MOTOR