#include <Arduino.h>
#include "state_controller.h"

// See "DEFAULT_SETTINGS.h" for a list of valid settings

StateController* device = &StateController::getInstance();

void setup() {
    // Serial.begin(115200);
    device->set_rgbLED(0, 0, 0);
    device->go_to_state(PROGRAMMING_STATE);
}

void loop() {
    device->update();
}
