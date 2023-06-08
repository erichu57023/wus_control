// #include <Arduino.h>
#include "state_controller.h"
#include "advertising_state.h"
#include "programming_state.h"

// See "DEFAULT_SETTINGS.h" for a list of valid settings

StateController device;

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    AdvertisingState adState(device);
    ProgrammingState progState(device);
   
    device.add_state(adState);
    device.add_state(progState);

    //  device.add_state(InterruptState());
    //  device.add_state(BurstingState());
    //  device.add_state(IdleState());

    device.go_to_state(ADVERTISING_STATE);
}

void loop() {
    device.update();
}
