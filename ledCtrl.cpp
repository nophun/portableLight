#include <Arduino.h>
#include "ledCtrl.h"

LedCtrl::LedCtrl(unsigned int led_ch, unsigned int bits, unsigned int init_brightness, float step) : 
    m_led_ch{led_ch}, m_brightness{init_brightness}, m_step{step} {
    m_request = init_brightness;
    
    m_max_brightness = 1 << bits;
    m_last_step = (1 << (bits - 1)) - 1;
}

void LedCtrl::init(void) {
    ledcWrite(m_led_ch, m_brightness);
}

void LedCtrl::request_brightness(unsigned int brightness) {
    if (brightness > m_last_step) {
        m_request = m_max_brightness;
    } else if (brightness == m_first_step) {
        m_request = m_min_brightness;
    } else {
        m_request = brightness;
    }
}

void LedCtrl::update(void) {
    /* update every 10th step */
    if (m_counter++ % 10 != 0) {
        return;
    }
    int diff = m_request - m_brightness;
    if (diff == 0) {
        return;
    }
    int increment = diff * m_step;
    if (increment == 0) {
        diff > 0 ? increment = 1 : increment = -1;
    }

    m_brightness += increment;
    ledcWrite(m_led_ch, m_brightness);
}
