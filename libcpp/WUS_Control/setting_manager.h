#ifndef SETTING_MANAGER_H
#define SETTING_MANAGER_H

class SettingManager {
    public:
        unsigned short int pwm_wave_gen;
        unsigned short int cs_tuss4470;
        unsigned short int cs_ad9833;
        unsigned short int cs_burst_control;
        unsigned short int voltage;
        unsigned int frequency;
        unsigned int timeout;
        unsigned int burst_period;
        unsigned int pulse_count;
        unsigned int duty_cycle;
        bool regulated_mode;
        bool pre_driver_mode;
        bool current_mode;
        bool io_mode;

        SettingManager();
};

#endif
