//========================
// WUS Controller
//========================

#include "AD9833.h"
#include "TUSS4470.h"
#define PIN_CS0 10          // into FSYNC on AD9833
#define PIN_CS1 9           // into NCS on TUSS4470
#define LED_OUT 7           // into an LED, displays stimOn
#define BURST_ON 6          // taken LOW to signal start of burst

//----------------------------
bool stimOn = false;        // stumulation on/off
float stimFreq = 440000;    // pulse frequency in Hz

//unsigned long PL = 1e3;     // pulse length in us
//unsigned long PP = 2e3;     // pulse period in us
unsigned long BL = 10e3;    // burst length in us; if using pulse_count, make sure this is longer than the total burst time
unsigned long BP = 20e3;   // burst period in us
unsigned long MAXD = 999e6; // max duration of stimulation in us

bool pre_driver_mode = true; 
bool regulated_mode = true;    // turn off to directly drive transducer with external power supply
uint8_t burst_voltage = 10; // regulated burst voltage (min 5, max 20)
CurrentType burst_current = HIGH_CURRENT;  // can be high or low
IOMode io_mode = IO_MODE1;  // see datasheet for details on IO mode
uint8_t pulse_count = 20;    // # of pulses per burst (max 63, 0 = continuous)
//----------------------------

unsigned long t_start;
String input;
char inChar;

AD9833 sig_gen(PIN_CS0, 25000000UL);
TUSS4470 burst_gen(PIN_CS1, pre_driver_mode);
//========================================
void setup() {
  pinMode(LED_OUT, OUTPUT); digitalWrite(LED_OUT, LOW);
  pinMode(BURST_ON, OUTPUT); digitalWrite(BURST_ON, HIGH);
  Serial.begin(38400); Serial.setTimeout(10);

  sig_gen.Begin();
  sig_gen.ApplySignal(SQUARE_WAVE, REG0, stimFreq);
  sig_gen.EnableOutput(true);
  
  burst_gen.begin();
  burst_gen.set(burst_voltage, burst_current, io_mode, pulse_count);
  if (regulated_mode) {burst_gen.enableRegulation();}
  if (pre_driver_mode) {burst_gen.enablePreDriver();}
}

//========================================
void loop() {
  if (stimOn){burst();}
}

//void pulse() {
//  static unsigned long t_pulse;  
//  t_pulse = micros();
//  sig_gen.EnableOutput(true);
//  while (micros() - t_pulse <= PL) {}
//
//  sig_gen.EnableOutput(false);
//  while (micros() - t_pulse <= PP) {}
//
//  serialEvent();
//}

void burst() {
  static unsigned long t_burst; 
  digitalWrite(BURST_ON, LOW); 
  digitalWrite(LED_OUT, HIGH);
  t_burst = micros();
  while (stimOn && (micros() - t_burst <= BL)) {
    if (micros() - t_start > MAXD) {reset(); return;}
//    pulse();
  }

  digitalWrite(BURST_ON, HIGH);
  digitalWrite(LED_OUT, LOW);
  while (stimOn && (micros() - t_burst <= BP)) {
    if (micros() - t_start > MAXD) {reset(); return;}
  }
}

void reset() {
  digitalWrite(LED_OUT, LOW);
  digitalWrite(BURST_ON, HIGH);
  stimOn = false;
//  sig_gen.EnableOutput(false);
}

void serialEvent() {
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();
    parseInput();
  }
}

void parseInput() {
  if (input == "off") {                         // turn off
    reset();
  } else if (input == "on") {                   // turn on
    stimOn = true;
    t_start = micros();
  } else if (input.startsWith("f=")) {          // change frequency
    stimFreq = input.substring(2).toFloat();
    sig_gen.ApplySignal(SQUARE_WAVE, REG0, stimFreq);
//  } else if (input.startsWith("pl=")) {         // change pulse length
//    PL = (unsigned long)input.substring(3).toInt();
//  } else if (input.startsWith("pp=")) {         // change pulse period
//    PP = (unsigned long)input.substring(3).toInt();
  } else if (input.startsWith("bl=")) {         // change burst length
    BL = (unsigned long)input.substring(3).toInt();
  } else if (input.startsWith("bp=")) {         // change burst period
    BP = (unsigned long)input.substring(3).toInt();
  } else if (input.startsWith("maxd=")) {       // change max duration
    MAXD = (unsigned long)input.substring(5).toInt();
  } else if (input.startsWith("pc=")) {
    reset();
    pulse_count = (uint8_t)input.substring(3).toInt(); // change pulse_count
    burst_gen.setPulseCount(pulse_count);
  } else if (input.startsWith("volt=")) {
    reset();
    burst_voltage = (uint8_t)input.substring(5).toInt(); // change pulse_count
    burst_gen.setVoltage(burst_voltage);
  }
  
}
