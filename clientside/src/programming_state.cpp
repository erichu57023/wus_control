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
    uint32_t val = ctrl->reprogramValue;
    // Bit-cast to float if setting is a float by default
    float valFloat;
    memcpy(&valFloat, &val, 4);

    switch (ctrl->reprogramSetting) {
        case BURSTPD_CHG:
            ctrl->settings->burst_pd = val;
            break;
            
        case BURSTDC_CHG:
            ctrl->settings->burst_dc = valFloat;
            break;

        case STIMPD_CHG:
            ctrl->settings->stim_pd = val;
            break;

        case STIMDC_CHG:
            ctrl->settings->stim_dc = valFloat;
            break;
        
        case FREQ_CHG:
            ctrl->settings->frequency = val;
            ctrl->waveGen->SetFrequency(REG0, val);
            break;
        
        case TOUT_CHG:
            ctrl->settings->timeout = val;
            break;

        case VOLT_CHG: 
            ctrl->settings->voltage = val;
            ctrl->burstGen->setVoltage(val);
            break;

        case PULSECT_CHG: 
            ctrl->settings->pulse_count = val;
            ctrl->burstGen->setPulseCount(val);
            break;

        case NO_CHG:
            break;    
    }
}
