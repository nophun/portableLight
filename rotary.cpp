#include <Arduino.h>
#include "rotary.h"

static const unsigned char fullStepsTable[7][4] = {
    // 00           01              10              11
    {R_START,       FS_R_CW_BEGIN,  FS_R_CCW_BEGIN, R_START},           // R_START
    {FS_R_CW_NEXT,  R_START,        FS_R_CW_FINAL,  R_START | DIR_CW},  // R_CW_FINAL
    {FS_R_CW_NEXT,  FS_R_CW_BEGIN,  R_START,        R_START},           // R_CW_BEGIN
    {FS_R_CW_NEXT,  FS_R_CW_BEGIN,  FS_R_CW_FINAL,  R_START},           // R_CW_NEXT
    {FS_R_CCW_NEXT, R_START,        FS_R_CCW_BEGIN, R_START},           // R_CCW_BEGIN
    {FS_R_CCW_NEXT, FS_R_CCW_FINAL, R_START,        R_START | DIR_CCW}, // R_CCW_FINAL
    {FS_R_CCW_NEXT, FS_R_CCW_FINAL, FS_R_CCW_BEGIN, R_START}            // R_CCW_NEXT
};

static const unsigned char halfStepsTable[6][4] = {
    // 00                  01              10                       11
    {HS_R_START_M,           HS_R_CW_BEGIN,     HS_R_CCW_BEGIN,     R_START},           // R_START (00)
    {HS_R_START_M | DIR_CCW, R_START,           HS_R_CCW_BEGIN,     R_START},           // R_CCW_BEGIN
    {HS_R_START_M | DIR_CW,  HS_R_CW_BEGIN,     R_START,            R_START},           // R_CW_BEGIN
    {HS_R_START_M,           HS_R_CCW_BEGIN_M,  HS_R_CW_BEGIN_M,    R_START},           // R_START_M (11)
    {HS_R_START_M,           HS_R_START_M,      HS_R_CW_BEGIN_M,    R_START | DIR_CW},  // R_CW_BEGIN_M 
    {HS_R_START_M,           HS_R_CCW_BEGIN_M,  HS_R_START_M,       R_START | DIR_CCW}  // R_CCW_BEGIN_M
};

RotaryEncoder::RotaryEncoder(uint16_t clock_index, uint16_t data_index, uint32_t timeout, RotaryMode mode) : 
m_clock_index(clock_index), m_data_index(data_index), m_timeout(timeout), m_mode(mode) {
    assert(clock_index != data_index);
}

uint8_t RotaryEncoder::update(void) {
    // uint8_t buf_idx = m_clock_index / I16_BITS;   // buffer selection
    bool clock_active = digitalRead(m_clock_index);
    bool data_active = digitalRead(m_data_index);

    /* Clear after timeout */
    if (m_change_time != 0U && millis() - m_change_time > m_timeout) {
        // active[buf_idx] &= ~BV(m_clock_index % I16_BITS);
        // active[buf_idx] &= ~BV(m_data_index % I16_BITS);
        m_change_time = 0U;
        return 0U;
    }

    /* Check encoder steps */
    if (m_mode == RotaryMode::HALF_STEP) {
        m_input_last_state = halfStepsTable[m_input_last_state & STEP_MASK][(data_active << 1) | clock_active];
    } else {
        m_input_last_state = fullStepsTable[m_input_last_state & STEP_MASK][(data_active << 1) | clock_active];
    }
    uint8_t direction = (m_input_last_state & DIR_MASK);

    if (direction == DIR_CW) {
        // active[buf_idx] |= BV(m_clock_index % I16_BITS);
        m_change_time = millis();
    } else if (direction == DIR_CCW) {
        // active[buf_idx] |= BV(m_data_index % I16_BITS);
        m_change_time = millis();
    }

    return direction;
}