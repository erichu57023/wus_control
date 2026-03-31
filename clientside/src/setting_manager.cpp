#include "setting_manager.h"

volatile uint8_t  SettingManager :: voltage     = VOLTAGE;
volatile uint8_t  SettingManager :: pulse_count = PULSE_COUNT;
volatile float    SettingManager :: frequency   = FREQUENCY;
volatile uint32_t SettingManager :: timeout     = TIMEOUT;
volatile uint32_t SettingManager :: burst_pd    = BURST_PD;
volatile uint32_t SettingManager :: stim_pd     = STIM_PD;
volatile float    SettingManager :: burst_dc    = BURST_DC;
volatile float    SettingManager :: stim_dc     = STIM_DC;
volatile uint8_t  SettingManager :: dc_seq_len  = arraysize(DC_SEQ_VALS);
volatile uint8_t  SettingManager :: dc_seq_vals[256];

volatile mutableSetting SettingManager :: repgmKey = NO_CHG; 
volatile uint32_t       SettingManager :: repgmVal = 0;

SettingManager :: SettingManager(void) {
    // Must copy data over dynamically
    memcpy((void*) dc_seq_vals, DC_SEQ_VALS, arraysize(DC_SEQ_VALS)); 
}
