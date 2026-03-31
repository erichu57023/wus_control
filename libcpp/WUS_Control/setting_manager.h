#ifndef SETTING_MANAGER_H
#define SETTING_MANAGER_H

#include <stdint.h>
#include "TUSS4470.h"

class SettingManager {
    public:
        uint8_t pwm_wave_gen;
        uint8_t cs_tuss4470;
        uint8_t cs_ad9833;
        uint8_t cs_burst_control;
        uint8_t voltage;
        uint8_t pulse_count;

        uint8_t duty_cycle;

        uint32_t frequency;
        uint32_t timeout;
        uint32_t burst_period;

        bool regulated_mode;

        IOMode io_mode;
        CurrentType current_mode;
        PreDriverMode pre_driver_mode;

        SettingManager(void);
};

#endif
