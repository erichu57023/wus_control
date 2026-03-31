#include "state_controller.h"

StateController :: StateController()
        : devStatus(DEVICE_OK) {
    load_settings();
    setup_rgbLED();
}

void StateController :: add_state(State* state) {
    stateList[state->name] = state;
}
 
void StateController :: go_to_state(stateName name) {
    if (currentState->name != NULL_STATE) {
        previousState = currentState->name;
        currentState->exit();
    }
    currentState = stateList[name];
    currentState->enter();
}

void StateController :: update() {
    currentState->update();
}

void StateController :: set_rgbLED(int rVal, int gVal, int bVal) {
    rgbLED->setPixelColor(0, rVal, gVal, bVal);
    rgbLED->show();
}

bool StateController :: is_connected() {
    return Bluefruit.connected();
}

void StateController :: load_settings() {
    *settings = SettingManager();
}

void StateController :: setup_rgbLED() {
    #define NUMPIXELS 1
    #define DATAPIN 8
    #define CLOCKPIN 6
    *rgbLED = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
    rgbLED->begin();
    rgbLED->setBrightness(10);
    rgbLED->show();
}
