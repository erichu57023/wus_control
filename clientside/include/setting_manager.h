#ifndef SETTING_MANAGER_H
#define SETTING_MANAGER_H

#include <stdint.h>
#include "TUSS4470.h"
#include "DEFAULT_SETTINGS.h"

#define arraysize(A) (sizeof(A)/sizeof(A[0]))
enum mutableSetting : uint16_t {
    NO_CHG = 0, 
    VOLT_CHG = VOLTAGE_UUID,
    PULSECT_CHG = PULSE_CT_UUID,
    FREQ_CHG = FREQUENCY_UUID, 
    TOUT_CHG = TIMEOUT_UUID,
    BURSTPD_CHG = BURST_PD_UUID, 
    BURSTDC_CHG = BURST_DC_UUID, 
    STIMPD_CHG = STIM_PD_UUID, 
    STIMDC_CHG = STIM_DC_UUID, 
    DCSEQ_CHG = DCSEQ_UUID
};
const uint8_t nrf52840_port_map[26] = {25, 24, 34,  6, 29, 27, 41, 40,  8,  7,  // Arduino pins 0-9
                                        5, 26, 11, 12,  4, 30, 28, 31,  2,  3,  // Arduino pins 10-19
                                      255, 16, 14, 20, 15, 13};                 // Arduino pins 20-25

class SettingManager {
    public:
        // Singleton constructor methods
        static SettingManager& getInstance() {
            static SettingManager singleton;
            return singleton;
        }
        SettingManager(SettingManager const& other) = delete;
        SettingManager& operator = (const SettingManager& other) = delete;

        // Constant instance variables
        static const uint8_t       cs_tuss4470      = CS_TUSS4470;
        static const uint8_t       cs_ad9833        = CS_AD9833;
        static const uint8_t       cs_burst_control = CS_BURST_CONTROL;
        static const bool          regulated_mode   = REGULATED_MODE;
        static const IOMode        io_mode          = static_cast<IOMode>(IO_MODE);
        static const CurrentType   current_mode     = static_cast<CurrentType>(CURRENT_MODE);      
        static const PreDriverMode pre_driver_mode  = static_cast<PreDriverMode>(PRE_DRIVER_MODE);

        // Mutable instance variables
        static volatile uint8_t    voltage;
        static volatile uint8_t    pulse_count;        
        static volatile float      frequency;
        static volatile uint32_t   timeout;
        static volatile uint32_t   burst_pd;
        static volatile uint32_t   stim_pd;
        static volatile float      burst_dc;
        static volatile float      stim_dc;
        static volatile uint8_t    dc_seq_len;
        static volatile uint8_t    dc_seq_vals[256];

        static volatile mutableSetting repgmKey;
        static volatile uint32_t       repgmVal;
    
    private:
        // Constructor
        SettingManager(void);
};

#endif
