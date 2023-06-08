#include "setting_manager.h"
#include "DEFAULT_SETTINGS.h"

SettingManager :: SettingManager(void) :
        pwm_wave_gen(           static_cast<uint8_t>            (PWM_WAVE_GEN)),
        cs_tuss4470(            static_cast<uint8_t>            (CS_TUSS4470)),
        cs_ad9833(              static_cast<uint8_t>            (CS_AD9833)),
        cs_burst_control(       static_cast<uint8_t>            (CS_BURST_CONTROL)),
        voltage(                static_cast<uint8_t>            (VOLTAGE)), 
        pulse_count(            static_cast<uint8_t>            (PULSE_COUNT)), 
        duty_cycle(             static_cast<uint8_t>            (DUTY_CYCLE)),
        frequency(              static_cast<uint32_t>           (FREQUENCY)), 
        timeout(                static_cast<uint32_t>           (TIMEOUT)), 
        burst_period(           static_cast<uint32_t>           (BURST_PERIOD)),
        regulated_mode(         static_cast<bool>               (REGULATED_MODE)), 
        io_mode(                static_cast<IOMode>             (IO_MODE)),
        current_mode(           static_cast<CurrentType>        (CURRENT_MODE)),
        pre_driver_mode(        static_cast<PreDriverMode>      (PRE_DRIVER_MODE)) {}
