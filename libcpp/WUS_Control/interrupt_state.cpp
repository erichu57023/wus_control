#include "interrupt_state.h"

stateName InterruptState :: get_name(void) {
    return IDLE_STATE;
}

void InterruptState :: enter(StateController* ctrl) {}

void InterruptState :: exit(StateController* ctrl) {}

void InterruptState :: update(StateController* ctrl) {
    if (ctrl->devStatus == DEVICE_INTERRUPT) {
        this->read_input_buffer(ctrl);
        this->parse_command(ctrl);
    } else {
        ctrl->devStatus = DEVICE_OK; 
        ctrl->go_to_state(IdleState::getInstance());
    }
}

void InterruptState :: read_input_buffer(StateController* ctrl) {
    uint8_t idx = 0;
    char charIn; 
    while (ctrl->bleuart.available()) {
        if (idx > 63) {break;}
        charIn = ctrl->bleuart.read();
        
        if (charIn == '\n') {           // End on newline by null-terminating string
            readBuffer[idx] = 0;
            break;
        } else if (isspace(charIn)) {   // Skip whitespace
            continue;
        } 

        if (isupper(charIn)) {
            charIn = tolower(charIn);
        }

        readBuffer[idx] = charIn;
        idx++;
    }
}

void InterruptState :: parse_command(StateController* ctrl) {
    char* command = strtok(readBuffer, "=");
    char* value = strtok(NULL, "=");
    
    if (!value) {
        if (str_equals(command, "on")) {
            ctrl->go_to_state(BurstingState::getInstance());
        } else if (str_equals(command, "off")) {
            ctrl->go_to_state(IdleState::getInstance());
        }
    } else {
        if (str_equals(command, "frequency")) {
            ctrl->reprogramSetting = FREQ_CHG;
        } else if (str_equals(command, "burst_pd")) {
            ctrl->reprogramSetting = BURSTPD_CHG;
        } else if (str_equals(command, "burst_dc")) {
            ctrl->reprogramSetting = BURSTDC_CHG;
        } else if (str_equals(command, "stim_pd")) {
            ctrl->reprogramSetting = STIMPD_CHG;
        } else if (str_equals(command, "stim_dc")) {
            ctrl->reprogramSetting = STIMDC_CHG;
        } else if (str_equals(command, "timeout")) {
            ctrl->reprogramSetting = TOUT_CHG;
        } else if (str_equals(command, "voltage")) {
            ctrl->reprogramSetting = VOLT_CHG;
        } else if (str_equals(command, "pulse_count")) {
            ctrl->reprogramSetting = PULSECT_CHG;
        } else {
            // Serial.println("No such command!");
        }
        ctrl->reprogramValue = atof(value);
        ctrl->go_to_state(ProgrammingState::getInstance());
    }
}

static bool str_equals(const char* strA, const char* strB) {
    return (!strcmp(strA, strB));
}
