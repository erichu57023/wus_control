/*
   TUSS4470.h - Library for controlling ultrasound burst excitation with TUSS4470. Listening is not supported.
   Author: Eric Hu, 1/10/2022
*/

#ifndef TUSS4470_H
#define TUSS4470_H

#include <Arduino.h>
#include <SPI.h>
#include "nrf.h"
#include "nrf_gpio.h"

typedef enum {READ = 0x8000, WRITE = 0x0000} CommandType;
typedef enum {LOW_CURRENT = 0, HIGH_CURRENT = 1} CurrentType;
typedef enum {HIZ_OFF = 0, HIZ_ON = 1} HiZType;
typedef enum {IO_MODE0 = 0, IO_MODE1 = 1, IO_MODE2 = 2, IO_MODE3 = 3} IOMode;
typedef enum {PRE_DRV0 = 0, PRE_DRV1 = 1} PreDriverMode;
typedef enum {STDBY_OFF = 0, STDBY_ON = 1} StandbyMode;

#include "setting_manager.h"

const uint8_t RESET_ADDS[] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x17, 0x18, 0x1A, 0x1B};
const uint8_t RESET_DATA[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x07, 0x14, 0x00, 0x40};

class TUSS4470 {
  public:
    // Initialize with controller's chip-select pin
    TUSS4470(uint8_t CSpin);

    // Run immediately after creating TUSS4470 object
    void begin();

    // Resets the TUSS4470 to its startup defaults
    void reset();

    // Disconnects the regulated VDRV voltage from the power supply
    void disableRegulation();
    
    // Connects the regulated VDRV voltage to the power supply
    void enableRegulation();

    // Disables pre-driver mode 
    void disablePreDriver();
    
    // Enables pre-driver mode 
    void enablePreDriver();

    // Disables standby mode
    void disableStandbyMode();

    // Enables standby mode
    void enableStandbyMode();
    
    // Sets voltage, current, IO mode, and pulse count
    void set(uint8_t voltage, CurrentType current, IOMode mode, uint8_t pulse_count);

    // Sets the VDRV regulated voltage (Min 5, Max 20)
    void setVoltage(uint8_t volt);

    // Sets the VDRV charging current (Allowed: LOW_CURRENT, HIGH_CURRENT)
    void setCurrent(CurrentType curr);

    // Sets the behavior of the IO pins; see datasheet (Allowed: 0-3)
    void setIOMode(IOMode mode);

    // Sets the number of pulses generated before burst shutoff; continuous if set to 0 (Max 63);
    void setPulseCount(uint8_t count);


  private:
    void updateVDRVRegister();
    void updateBurstPulseRegister();
    void updateTOFConfigRegister();
    void updateDevConfigRegister();
    void controlRegister(CommandType rw, byte address, byte data);
    bool findParity16(uint16_t num);
    void transferCommand(uint16_t data16);

    uint8_t _CSpin, _drvVoltage, _pulseCount;
    PreDriverMode _preDriver;
    CurrentType _drvCurrent;
    HiZType _drvHiZ;
    StandbyMode _standbyMode;
    IOMode _ioMode;
};

#endif
