#pragma once

#include <Arduino.h>

#define DIR_MASK        0x30
#define STEP_MASK       0x0F

/* Direction flags */
#define R_START         0x00
#define DIR_CW          0x10
#define DIR_CCW         0x20

/* Full step step flags */
#define FS_R_CW_FINAL   0x01
#define FS_R_CW_BEGIN   0x02
#define FS_R_CW_NEXT    0x03
#define FS_R_CCW_BEGIN  0x04
#define FS_R_CCW_FINAL  0x05
#define FS_R_CCW_NEXT   0x06

/* Half step step flags */
#define HS_R_CCW_BEGIN   0x1
#define HS_R_CW_BEGIN    0x2
#define HS_R_START_M     0x3
#define HS_R_CW_BEGIN_M  0x4
#define HS_R_CCW_BEGIN_M 0x5

enum class RotaryMode {
    HALF_STEP = 0,
    FULL_STEP = 1
};

class RotaryEncoder {
public:
    RotaryEncoder(uint16_t clock_index, uint16_t data_index, uint32_t timeout, RotaryMode mode = RotaryMode::FULL_STEP);
    ~RotaryEncoder() = default;
    RotaryEncoder(const RotaryEncoder &other) = delete;
    RotaryEncoder(RotaryEncoder &&other) = delete;
    RotaryEncoder &operator=(const RotaryEncoder &other) = delete;
    RotaryEncoder &operator=(RotaryEncoder &&other) = delete;

    void set_mode(RotaryMode mode);
    void set_timeout(uint32_t timeout);
    uint8_t update(void);
private:
    uint16_t m_clock_index {0};
    uint16_t m_data_index {0};
    uint32_t m_timeout {1};
    uint32_t m_change_time {0};
    uint8_t m_input_last_state {0};
    RotaryMode m_mode {RotaryMode::HALF_STEP};
};