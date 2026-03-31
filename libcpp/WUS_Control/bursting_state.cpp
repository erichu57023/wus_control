#include "bursting_state.h"

stateName BurstingState :: get_name(void) {
    return BURSTING_STATE;
}

void BurstingState :: enter(StateController* ctrl) {
    Serial.println("bursting");

    ctrl->set_rgbLED(0, 0, 255); // blue
    this->set_parameters(ctrl);
    this->burstStartTime = micros();
    this->pulseStartTime = this->burstStartTime;
}

void BurstingState :: exit(StateController* ctrl) {
    this->pulseOff();
}

void BurstingState :: update(StateController* ctrl) {
    if (this->burst_elapsed() > this->timeout) {         // timeout reached, return to idle
        ctrl->go_to_state(IdleState::getInstance()); 
    } else {                                                   // continue current burst
        if (this->pulse_elapsed() > this->pulsePeriod) {      // start pulse if new period reached
            this->pulseStartTime = micros();
            this->pulseOn();
        } else if (this->pulse_elapsed() > this->onDuration) { // stop pulse if on duration reached
            this->pulseOff();
        }
    }
}

void BurstingState :: set_parameters(StateController* ctrl) {
    // Use hardware port value for fast toggle
    this->ctrlPort = nrf52840_port_map[ctrl->settings->cs_burst_control];

    nrf_gpio_cfg_output(this->ctrlPort);
    this->pulseOff();
    this->timeout = ctrl->settings->timeout * 1e6;            // time in us
    this->pulsePeriod = ctrl->settings->burst_period * 1e3;   // time in us
    if (!ctrl->settings->pulse_count) {
        this->dutyCycle = ctrl->settings->duty_cycle;          // duty cycle in percent
    }
    this->onDuration = this->pulsePeriod * this->dutyCycle;
}

void BurstingState :: pulseOn(void) {
    nrf_gpio_pin_set(this->ctrlPort);
}

void BurstingState :: pulseOff(void) {
    nrf_gpio_pin_clear(this->ctrlPort);
}

uint32_t BurstingState :: burst_elapsed(void) {
    return (micros() - this->burstStartTime);
}

uint32_t BurstingState :: pulse_elapsed(void) {
    return (micros() - this->pulseStartTime);
}
