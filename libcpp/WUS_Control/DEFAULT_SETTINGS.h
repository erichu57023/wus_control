#ifndef DEFAULT_SETTINGS_H
#define DEFAULT_SETTINGS_H

/* ------------ POWER OPTIONS ------------ */

// the pin where baseline frequency is output
#define PWM_WAVE_GEN 11

// the chip-select pin for the TUSS4470
#define CS_TUSS4470 12

// the chip-select pin for the AD9833
#define CS_AD9833 13

// the pin that controls bursting of the TUSS4470
#define CS_BURST_CONTROL 2

// regulated mode, turns on TUSS4470 internal voltage regulator (1 = on, 0 = off)
#define REGULATED_MODE 1

// pre-driver mode, use only if driving external transistors with the TUSS4470 (1 = on, 0 = off)
#define PRE_DRIVER_MODE 0

// output voltage (only matters if internal regulator is on) (minimum 5, maximum 21)
#define VOLTAGE 10

// current mode, used to charge the capacitor if using regulated mode (low = 0, high = 1)
#define CURRENT_MODE 1

// IO mode, check TUSS4470 datasheet for more info 
#define IO_MODE 1

/* ------------ BURST OPTIONS ------------ */

// fundamental frequency in Hz
#define FREQUENCY 450000

// maximum duration of burst stimulation in seconds
#define TIMEOUT 1

// burst period in milliseconds
#define BURST_PERIOD 10

// number of pulses per burst; continuous if set to 0; maximum of 63
#define PULSE_COUNT 0

// burst duty cycle percentage, ONLY matters if PULSE_COUNT = 0
#define DUTY_CYCLE 50

/* --------------------------------------- */

#endif
