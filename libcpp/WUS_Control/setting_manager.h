#ifndef SETTING_MANAGER_H
#define SETTING_MANAGER_H

#include <stdint.h>
#include "TUSS4470.h"

enum mutableSetting {NO_CHG, VOLT_CHG, PULSECT_CHG, DUTY_CHG, FREQ_CHG, TOUT_CHG, BURSTPD_CHG};

class SettingManager {
    public:
        // Singleton constructor methods
        static SettingManager& getInstance() {
            static SettingManager singleton;
            return singleton;
        }
        SettingManager(SettingManager const& other) = delete;
        SettingManager& operator = (const SettingManager& other) = delete;

        // Instance variables
        const uint8_t pwm_wave_gen;
        const uint8_t cs_tuss4470;
        const uint8_t cs_ad9833;
        const uint8_t cs_burst_control;
        volatile uint8_t voltage;
        volatile uint8_t pulse_count;
        volatile uint8_t duty_cycle;
        volatile uint32_t frequency;
        volatile uint32_t timeout;
        volatile uint32_t burst_period;
        const bool regulated_mode;
        const IOMode io_mode;
        const CurrentType current_mode;
        const PreDriverMode pre_driver_mode;
    
    private:
        // Constructor
        SettingManager(void);
};

#endif
