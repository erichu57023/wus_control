#include "idle_state.h"

stateName IdleState :: get_name(void) {
    return IDLE_STATE;
}

void IdleState :: enter(StateController* ctrl) {
    Serial.println("idle");
    ctrl->set_rgbLED(0, 255, 0); // green
}

void IdleState :: exit(StateController* ctrl) {}

void IdleState :: update(StateController* ctrl) {}
