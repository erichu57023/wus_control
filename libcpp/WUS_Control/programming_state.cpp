#include "programming_state.h"

stateName ProgrammingState :: get_name(void) {
    return PROGRAMMING_STATE;
}

void ProgrammingState :: enter(StateController* ctrl) {
    // Serial.println("programming");
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
  if (this->initialized) {
    ctrl->go_to_state(IdleState::getInstance());
  } else {
    this->initialized = true;
    ctrl->go_to_state(AdvertisingState::getInstance());
  }
}

void ProgrammingState :: startup_sequence(StateController* ctrl) {
    #define refFreq 25000000UL
    
    // this->gpio_clock_8m(ctrl->settings->pwm_wave_gen);
    ctrl->waveGen = new AD9833(ctrl->settings->cs_ad9833, refFreq);
    ctrl->burstGen = new TUSS4470(ctrl->settings->cs_tuss4470);

    ctrl->waveGen->Begin();
    ctrl->waveGen->DisableDAC(true);
    ctrl->waveGen->ApplySignal(SQUARE_WAVE, REG0, ctrl->settings->frequency);

    ctrl->burstGen->begin();
    ctrl->burstGen->set(ctrl->settings->voltage, ctrl->settings->current_mode, 
                  ctrl->settings->io_mode, ctrl->settings->pulse_count);
    if (ctrl->settings->regulated_mode) {ctrl->burstGen->enableRegulation();}
    if (ctrl->settings->pre_driver_mode) {ctrl->burstGen->enablePreDriver();}
    ctrl->burstGen->enableStandbyMode();
}

void ProgrammingState :: change_setting(StateController* ctrl) {
    float val = ctrl->reprogramValue;
    switch (ctrl->reprogramSetting) {
      
        case BURSTPD_CHG:
            ctrl->settings->burst_pd = static_cast<uint32_t>(val);
            break;
              
        case BURSTDC_CHG:
            ctrl->settings->burst_dc = val;
            break;

        case STIMPD_CHG:
            ctrl->settings->stim_pd = static_cast<uint32_t>(val);
            break;

        case STIMDC_CHG:
            ctrl->settings->stim_dc = val;
            break;
        
        case FREQ_CHG:
            ctrl->settings->frequency = val;
            ctrl->waveGen->SetFrequency(REG0, val);
            break;
        
        case TOUT_CHG:
            ctrl->settings->timeout = static_cast<uint32_t>(val);
            break;

        case VOLT_CHG: 
            ctrl->settings->voltage = static_cast<uint8_t>(val);
            ctrl->burstGen->setVoltage(ctrl->settings->voltage);
            break;

        case PULSECT_CHG: 
            ctrl->settings->pulse_count = static_cast<uint8_t>(val);
            ctrl->burstGen->setPulseCount(ctrl->settings->pulse_count);
            break;

    }
}

// void ProgrammingState :: gpio_clock_8m(uint8_t pin) {
//     uint8_t pin_number = nrf52840_port_map[pin];
//     nrf_gpio_cfg_output(pin_number);
//     NRF_TIMER1->PRESCALER = 0; // 8MHz
//     NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
//     NRF_TIMER1->CC[0] = 1;

//     NRF_GPIOTE->CONFIG[0] = GPIOTE_CONFIG_MODE_Task | (pin_number << GPIOTE_CONFIG_PSEL_Pos) |
//                             (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);

//     /*Connect TIMER event to GPIOTE out task*/
//     NRF_PPI->CH[0].EEP = (uint32_t) &NRF_TIMER1->EVENTS_COMPARE[0];
//     NRF_PPI->CH[0].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[0];
//     NRF_PPI->CHENSET   = 1;

//     /*Starts clock signal*/
//     NRF_TIMER1->TASKS_START = 1;
// }
