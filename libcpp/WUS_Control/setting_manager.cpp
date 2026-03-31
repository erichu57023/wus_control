#include "setting_manager.h"
#include "DEFAULT_SETTINGS.h"

SettingManager :: SettingManager() :
        pwm_wave_gen((unsigned short int) PWM_WAVE_GEN),
        cs_tuss4470((unsigned short int) CS_TUSS4470),
        cs_ad9833((unsigned short int) CS_AD9833),
        cs_burst_control((unsigned short int) CS_BURST_CONTROL),
        voltage((unsigned short int) VOLTAGE), 
        frequency((unsigned int) FREQUENCY), 
        timeout((unsigned int) TIMEOUT), 
        burst_period((unsigned int) BURST_PERIOD),
        pulse_count((unsigned int) PULSE_COUNT), 
        duty_cycle((unsigned int) DUTY_CYCLE),
        regulated_mode((bool)REGULATED_MODE), 
        pre_driver_mode((bool)PRE_DRIVER_MODE), 
        current_mode((bool)CURRENT_MODE), 
        io_mode((bool)IO_MODE) {}
