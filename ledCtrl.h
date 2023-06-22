#pragma once

class LedCtrl {
public:
    explicit LedCtrl(unsigned int led_ch, unsigned int bits, unsigned int init_brightness, float step);
    ~LedCtrl();
    LedCtrl(const LedCtrl &other) = delete;
    LedCtrl(LedCtrl &&other) = delete;
    LedCtrl &operator=(const LedCtrl &other) = delete;
    LedCtrl &operator=(LedCtrl &&other) = delete;

    void init(void);
    void request_brightness(unsigned int brightness);
    void update(void);
    unsigned int get_last_step(void) {
        return m_last_step;
    }
    unsigned int get_first_step(void) {
        return m_first_step;
    }

private:
    unsigned int m_led_ch {0};
    unsigned int m_brightness {0};
    unsigned int m_request {0};
    float m_step {0};
    unsigned int m_max_brightness {0};
    unsigned int m_min_brightness {0};
    unsigned int m_last_step {0};
    unsigned int m_first_step {1};
    unsigned int m_counter {1};
};
