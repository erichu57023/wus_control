#ifndef DEFAULT_SETTINGS_H
#define DEFAULT_SETTINGS_H

// Randomly generated UUIDs for each setting
#define TUSS4470_UUID           0x288B
#define AD9833_UUID             0x98CE
#define BURST_CTRL_UUID         0x48FA
#define REGULATED_MODE_UUID     0x3883
#define PRE_DRIVER_MODE_UUID    0xF1E6
#define CURRENT_MODE_UUID       0x07C0
#define IO_MODE_UUID            0x20FC
#define VOLTAGE_UUID            0x99A1
#define PULSE_CT_UUID           0x162E
#define FREQUENCY_UUID          0xEA86
#define TIMEOUT_UUID            0x61D0
#define BURST_PD_UUID           0x6D6F
#define STIM_PD_UUID            0xD067
#define BURST_DC_UUID           0x7AD9
#define STIM_DC_UUID            0xCDE8

#include <Arduino.h>

/* ------------ POWER OPTIONS ------------ */

// the chip-select pin for the TUSS4470
const uint8_t   CS_TUSS4470 = 12;

// the chip-select pin for the AD9833
const uint8_t   CS_AD9833 = 13;

// the pin that controls bursting of the TUSS4470
const uint8_t   CS_BURST_CONTROL = 14;

// regulated mode, turns on TUSS4470 internal voltage regulator (1 = on, 0 = off)
const bool      REGULATED_MODE = 1;

// pre-driver mode, use only if driving external transistors with the TUSS4470 (1 = on, 0 = off)
const bool      PRE_DRIVER_MODE = 1;

// output voltage (only matters if internal regulator is on) (minimum 5, maximum 21)
const uint8_t   VOLTAGE = 5;

// current mode, used to charge the capacitor if using regulated mode (0 = low, 1 = high)
const bool      CURRENT_MODE = 1;

// IO mode, check TUSS4470 datasheet for more info 
const uint8_t   IO_MODE = 1;

/* ------------ BURST OPTIONS ------------ */

// fundamental frequency in Hz
const float     FREQUENCY = 450e3;

// maximum duration of burst stimulation in MILLIseconds
const uint32_t  TIMEOUT = 10e3;

// burst period in MICROseconds
const uint32_t  BURST_PD = 10e3;

// burst duty cycle percentage 0-100, ONLY matters if PULSE_COUNT = 0
const float     BURST_DC = 50.0;

// number of pulses per burst; continuous if set to 0; maximum of 63
const uint8_t   PULSE_COUNT = 0;

// stimulation period in MICROseconds
const uint32_t  STIM_PD = 5e6;

// stimulation duty cycle percentage 0-100
const float     STIM_DC = 10;

/* --------------------------------------- */

#endif
