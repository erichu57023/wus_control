#include "bursting_state.h"

stateName BurstingState :: get_name(void) {
    return BURSTING_STATE;
}

void BurstingState :: enter(StateController* ctrl) {
    // Serial.println("bursting");

    ctrl->set_rgbLED(0, 0, 255); // blue
    ctrl->burstGen->disableStandbyMode();
    this->set_parameters(ctrl);
    this->stimStartTime = micros();
    this->burstStartTime = this->stimStartTime;
    this->pulseOn();
}

void BurstingState :: exit(StateController* ctrl) {
    ctrl->burstGen->enableStandbyMode();
    this->pulseOff();
}

void BurstingState :: update(StateController* ctrl) {
    static unsigned long now, stim_now, burst_now;
    now = micros();
    burst_now = now - this->burstStartTime;
    stim_now = now - this->stimStartTime;
    
    if (stim_now > this->timeout) {         // timeout reached, return to idle
        ctrl->go_to_state(IdleState::getInstance()); 
    } else if (burst_now > this->stimPeriod) {
        ctrl->burstGen->disableStandbyMode();
        this->burstStartTime = now;
        this->pulseOn();
    } else if (this->burstActive && burst_now > this->stimOnTime) {
        ctrl->burstGen->enableStandbyMode();
        this->pulseOff();
    }
}

void BurstingState :: set_parameters(StateController* ctrl) {
    this->timeout = ctrl->settings->timeout * 1e3;            // time in us
    this->ctrlPin = ctrl->settings->cs_burst_control;

    if (!HwPWM0.checkPin(this->ctrlPin)) {
        HwPWM0.setResolution(15);
        HwPWM0.addPin(this->ctrlPin);
    }

    float newBurstFreq = 1000000.0f / ctrl->settings->burst_pd;
    float newBurstDC = ctrl->settings->burst_dc;
    if (this->burstFreq != newBurstFreq || this->burstDC != newBurstDC) {
        this->burstFreq = newBurstFreq;
        this->burstDC = newBurstDC;
        
        uint32_t countTop = 16000000 / this->burstFreq;
        uint8_t prescaler = 0;
        while (countTop > 32768UL * pow(2, prescaler)) {
            prescaler++;
        }

        uint32_t compareValue = countTop / pow(2, prescaler) - 1;
        this->dutyCycleComp = round(map(this->burstDC, 0, 100.0f, 0, compareValue));
        HwPWM0.setClockDiv((unsigned long) prescaler);
        HwPWM0.setMaxValue(compareValue);
    }
    this->stimPeriod = ctrl->settings->stim_pd;
    this->stimDC = ctrl->settings->stim_dc / 100;
    this->stimOnTime = this->stimPeriod * this->stimDC;
}

void BurstingState :: pulseOn(void) {
    HwPWM0.writePin(this->ctrlPin, this->dutyCycleComp, true);
    this->burstActive = true;
    nrf_gpio_pin_set(LED_PIN);
}

void BurstingState :: pulseOff(void) {
    HwPWM0.writePin(this->ctrlPin, 0, true);
    this->burstActive = false;
    nrf_gpio_pin_clear(LED_PIN);
}
