#include "hardware/AD9833.h"
#include "hardware/TUSS4470.h"
#include "state_controller.h"
#include "advertising_state.h"

// See "DEFAULT_SETTINGS.h" for a list of valid settings

StateController* device = new StateController();
AdvertisingState* adState = new AdvertisingState(device);

void setup() {
    device->add_state(adState);
//  device.add_state(ProgrammingState());
//  device.add_state(InterruptState());
//  device.add_state(BurstingState());
//  device.add_state(IdleState());

    device->go_to_state(ADVERTISING_STATE);
}

void loop() {
    device->update();
}
