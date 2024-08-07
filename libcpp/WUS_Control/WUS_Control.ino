#include <Arduino.h>
#include "state_controller.h"
#include "advertising_state.h"

// See "DEFAULT_SETTINGS.h" for a list of valid settings

StateController* device = &StateController::getInstance();

void setup() {
  device->set_rgbLED(0, 0, 0);
  device->go_to_state(ProgrammingState::getInstance());
}

void loop() {
  device->update();
}
