/*
   TUSS4470.cpp - Library for controlling ultrasound burst excitation with TUSS4470. Listening is not supported.
   Author: Eric Hu, 1/10/2022
*/

#include "TUSS4470.h"

/*
   Create a TUSS4470 object
*/
TUSS4470 :: TUSS4470(uint8_t CSpin, bool pre_driver) {
  // Chip select pin to enable SPI (active low)
  pinMode(CSpin, OUTPUT);
  digitalWrite(CSpin, HIGH);
  
  _CSpin = CSpin;
  _drvVoltage = 0;
  _drvCurrent = LOW_CURRENT;
  _drvHiZ = HIZ_ON;
  _pulseCount = 0;
  _ioMode = IO_MODE0;
  _preDriver = pre_driver;
}


// ----------------------------- General functions ----------------------------- //

void TUSS4470 :: begin() {
//  SPI.begin();
  delay(100);
}

void TUSS4470 :: reset() {
  for (int i = 0; i < 10; i++) {
    controlRegister(WRITE, RESET_ADDS[i], RESET_DATA[i]);
  }
  _drvVoltage = 0;
  _drvCurrent = LOW_CURRENT;
  _drvHiZ = HIZ_ON;
  _pulseCount = 0;
  _ioMode = IO_MODE0;
}

void TUSS4470 :: disableRegulation() {
  _drvHiZ = HIZ_ON;
  updateVRDVRegister();
}

void TUSS4470 :: enableRegulation() {
  _drvHiZ = HIZ_OFF;
  updateVRDVRegister();
}

void TUSS4470 :: disablePreDriver() {
  _preDriver = false;
  updateBurstPulseRegister();
}

void TUSS4470 :: enablePreDriver() {
  _preDriver = true;
  updateBurstPulseRegister();
}

void TUSS4470 :: set(uint8_t voltage, CurrentType current, IOMode mode, uint8_t pulse_count) {
  setVoltage(voltage);
  setCurrent(current);
  setIOMode(mode);
  setPulseCount(pulse_count);
}

void TUSS4470 :: setVoltage(uint8_t volt) {
  volt = (volt < 5) ? 0 : (volt - 5); // Set to 0 if under 5
  volt &= 0xF;  // Only count last 4 bits
  _drvVoltage = volt;
  updateVRDVRegister();
}

void TUSS4470 :: setCurrent(CurrentType curr) {
  _drvCurrent = curr;
  updateVRDVRegister();
}

void TUSS4470 :: setIOMode(IOMode mode) {
  _ioMode = mode;
  updateDevConfigRegister();
}

void TUSS4470 :: setPulseCount(uint8_t count) {
  count &= 0x3F; // Only count last 6 bits
  _pulseCount = count;
  updateBurstPulseRegister();
}


// ----------------------------- Private functions ----------------------------- //

void TUSS4470 :: updateVRDVRegister() {
  Serial.println("VDRV");
  uint8_t data = (_drvHiZ << 5) + (_drvCurrent << 4) + _drvVoltage;
  controlRegister(WRITE, 0x16, data);
}

void TUSS4470 :: updateBurstPulseRegister() {
  Serial.println("BurstPulse");
  uint8_t data = _pulseCount;
  if (_preDriver) {data += 0x40;}
  controlRegister(WRITE, 0x1A, data);
}

void TUSS4470 :: updateDevConfigRegister() {
  Serial.println("DevConfig");
  uint8_t data = _ioMode;
  controlRegister(WRITE, 0x14, data);
}

void TUSS4470 :: controlRegister(CommandType rw, byte address, byte data) {
  address &= 0x3F;
  uint16_t comm = rw;
  comm += ((uint16_t)address << 9);
  comm += data;
  if (!findParity16(comm)) {comm += 0x0100;}

  transferCommand(comm);
}

bool TUSS4470 :: findParity16(uint16_t num) {
  num ^= num >> 8;
  num ^= num >> 4;
  num ^= num >> 2; 
  num ^= num >> 1;
  return num & 1;
}

void TUSS4470 :: transferCommand(uint16_t data16) {
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE1));
  digitalWrite(_CSpin, LOW);
  uint16_t rec16 = SPI.transfer16(data16);
  digitalWrite(_CSpin, HIGH);
  Serial.println(data16, BIN);
  Serial.println(rec16, BIN);
  Serial.println();
}
