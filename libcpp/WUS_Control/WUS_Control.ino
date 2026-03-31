#include <Arduino.h>
#include "state_controller.h"
#include "advertising_state.h"

// See "DEFAULT_SETTINGS.h" for a list of valid settings

StateController* device = &StateController::getInstance();

void setup() {
    device->set_rgbLED(0, 0, 0);
    Serial.begin(115200);
    device->go_to_state(AdvertisingState::getInstance());
}

void loop() {
    device->update();
}
