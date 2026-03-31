#include "state.h"

void State :: enter() {}

void State :: exit() {}

void State :: update() {
    if (!ctrl->is_connected()) {
        ctrl->devStatus = DEVICE_NO_CONNECT;
    } else if (ctrl->bleuart->available()) {
        ctrl->devStatus = DEVICE_INTERRUPT;
    } else {
        ctrl->devStatus = DEVICE_OK;
    }
}
