#include "bursting_state.h"

stateName BurstingState :: get_name(void) {
    return BURSTING_STATE;
}

void BurstingState :: enter(StateController* ctrl) {
    // Serial.println("bursting");

    ctrl->set_rgbLED(0, 0, 255); // blue
    this->set_parameters(ctrl);
    this->burstStartTime = micros();
    this->pulseOn();
}

void BurstingState :: exit(StateController* ctrl) {
    this->pulseOff();
}

void BurstingState :: update(StateController* ctrl) {
    if (micros() - this->burstStartTime > this->timeout) {         // timeout reached, return to idle
        ctrl->go_to_state(IdleState::getInstance()); 
    }
}

void BurstingState :: set_parameters(StateController* ctrl) {
    this->timeout = ctrl->settings->timeout * 1e6;            // time in us
   
    this->ctrlPin = ctrl->settings->cs_burst_control;
    this->burstFreq = 1000.0f / (float) ctrl->settings->burst_period;
    this->dutyCycle = 100.0f - (float) ctrl->settings->duty_cycle;  // Invert because ctrlPin is active LOW
    
    if (!this->burst_ctrl) {
        this->burst_ctrl = new nRF52_PWM(this->ctrlPin, this->burstFreq, 100.0f);
    }
}

void BurstingState :: pulseOn(void) {
    this->burst_ctrl->setPWM(this->ctrlPin, this->burstFreq, this->dutyCycle);
}

void BurstingState :: pulseOff(void) {
    this->burst_ctrl->setPWM_manual(this->ctrlPin, 100.0f);
}

unsigned long BurstingState :: burst_elapsed(void) {
    return (micros() - this->burstStartTime);
}
