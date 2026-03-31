#include "programming_state.h"

stateName ProgrammingState :: get_name(void) {
    return PROGRAMMING_STATE;
}

void ProgrammingState :: enter(StateController* ctrl) {
    Serial.println("programming");
    ctrl->set_rgbLED(255, 255, 0); // yellow
    if (!this->initialized) {
        this->startup_sequence(ctrl);
    } else if (ctrl->reprogramSetting != NO_CHG){
        this->change_setting(ctrl);
    }
}

void ProgrammingState :: exit(StateController* ctrl) {
    ctrl->reprogramSetting = NO_CHG;
    ctrl->reprogramValue = 0;
    ctrl->devStatus = DEVICE_OK;
}

void ProgrammingState :: update(StateController* ctrl) {    
    ctrl->go_to_state(IdleState::getInstance());
}

void ProgrammingState :: startup_sequence(StateController* ctrl) {
    #define refFreq 8000000UL
    // wavePWM = new nRF52_PWM(ctrl->settings->pwm_wave_gen, refFreq, 50.0f);
    this->gpio_clock_8m(ctrl->settings->pwm_wave_gen);
    waveGen = new AD9833(ctrl->settings->cs_ad9833, refFreq);
    burstGen = new TUSS4470(ctrl->settings->cs_tuss4470);

    waveGen->Begin();
    waveGen->ApplySignal(SQUARE_WAVE, REG0, static_cast<float>(ctrl->settings->frequency));
    waveGen->EnableOutput(true);

    burstGen->begin();
    burstGen->set(ctrl->settings->voltage, ctrl->settings->current_mode, 
                  ctrl->settings->io_mode, ctrl->settings->pulse_count);
    if (ctrl->settings->regulated_mode) {burstGen->enableRegulation();}
    if (ctrl->settings->pre_driver_mode) {burstGen->enablePreDriver();}

    this->initialized = true;
}

void ProgrammingState :: change_setting(StateController* ctrl) {
    uint32_t val = ctrl->reprogramValue;
    switch (ctrl->reprogramSetting) {
        case VOLT_CHG: 
            ctrl->settings->voltage = static_cast<uint8_t>(val);
            burstGen->setVoltage(ctrl->settings->voltage);
            break;

        case PULSECT_CHG: 
            ctrl->settings->pulse_count = static_cast<uint8_t>(val);
            burstGen->setPulseCount(ctrl->settings->pulse_count);
            break;
        
        case DUTY_CHG:
            ctrl->settings->duty_cycle = static_cast<uint8_t>(val);
            break;
        
        case FREQ_CHG:
            ctrl->settings->frequency = val;
            waveGen->SetFrequency(REG0, static_cast<float>(val));
            break;
        
        case TOUT_CHG:
            ctrl->settings->timeout = val;
            break;
        
        case BURSTPD_CHG:
            ctrl->settings->burst_period = val;
            break;
    }
}

void ProgrammingState :: gpio_clock_8m(uint8_t pin) {
    uint8_t pin_number = nrf52840_port_map[pin];
    nrf_gpio_cfg_output(pin_number);
    NRF_TIMER1->PRESCALER = 0; // 8MHz
    NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
    NRF_TIMER1->CC[0] = 1;

    NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Task | (pin_number << GPIOTE_CONFIG_PSEL_Pos) |
                            (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);

    /*Connect TIMER event to GPIOTE out task*/
    NRF_PPI->CH[0].EEP = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[0];
    NRF_PPI->CH[0].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[0];
    NRF_PPI->CHENSET   = 1;

    /*Starts clock signal*/
    NRF_TIMER1->TASKS_START = 1;
}
