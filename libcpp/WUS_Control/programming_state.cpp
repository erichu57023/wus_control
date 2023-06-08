#include "programming_state.h"

ProgrammingState :: ProgrammingState(StateController& controller) {
    ctrl = controller;
    wavePWM = NULL;
    waveGen = NULL;
    burstGen = NULL;
}

stateName ProgrammingState :: get_name(void) {
    return PROGRAMMING_STATE;
}

void ProgrammingState :: enter(void) {
    Serial.println("entering prog");
    ctrl.set_rgbLED(0, 255, 255); // yellow
    if (waveGen == NULL) {
        startup_sequence();
    } else if (ctrl.reprogramSetting != ""){
        change_setting();
    }
}

void ProgrammingState :: exit(void) {
    Serial.println("exiting prog");
    ctrl.reprogramSetting = "";
    ctrl.reprogramValue = 0;
}

void ProgrammingState :: update(void) {
    if (!ctrl.is_connected()) {
        ctrl.devStatus = DEVICE_NO_CONNECT; 
        ctrl.go_to_state(ADVERTISING_STATE);
    } else {
        ctrl.go_to_state(IDLE_STATE);
    }
}

void ProgrammingState :: startup_sequence(void) {
    #define refFreq 25000000UL
    wavePWM = new nRF52_PWM(ctrl.settings->pwm_wave_gen, refFreq, 50.0f);
    waveGen = new AD9833(ctrl.settings->cs_ad9833, refFreq);
    burstGen = new TUSS4470(ctrl.settings->cs_tuss4470);

    waveGen->Begin();
    waveGen->ApplySignal(SQUARE_WAVE, REG0, static_cast<float>(ctrl.settings->frequency));

    burstGen->begin();
    burstGen->set(ctrl.settings->voltage, ctrl.settings->current_mode, 
                  ctrl.settings->io_mode, ctrl.settings->pulse_count);
    if (ctrl.settings->regulated_mode) {burstGen->enableRegulation();}
    if (ctrl.settings->pre_driver_mode) {burstGen->enablePreDriver();}
}

void ProgrammingState :: change_setting(void) {
    uint32_t val = ctrl.reprogramValue;

    if (ctrl.reprogramSetting == "frequency") {
        ctrl.settings->frequency = val;
        waveGen->SetFrequency(REG0, static_cast<float>(val));

    } else if (ctrl.reprogramSetting == "voltage") {
        ctrl.settings->voltage = static_cast<uint8_t>(val);
        burstGen->setVoltage(ctrl.settings->voltage);
        
    } else if (ctrl.reprogramSetting == "pulse_count") {
        ctrl.settings->pulse_count = static_cast<uint8_t>(val);
        burstGen->setPulseCount(ctrl.settings->pulse_count);
    }
}
