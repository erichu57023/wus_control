#include "state_controller.h"

StateController :: StateController(void) {
    load_settings();
    setup_rgbLED();
}

void StateController :: add_state(State& state) {
    stateList[state.get_name()] = &state;
}

void StateController :: go_to_state(stateName name) {
    if (currentState != NULL_STATE) {
        previousState = stateList[currentState]->get_name();
        stateList[currentState]->exit();
    }
    currentState = name;
    stateList[currentState]->enter();
}

void StateController :: update(void) {
    stateList[currentState]->update();
}

void StateController :: set_rgbLED(uint8_t rVal, uint8_t gVal, uint8_t bVal) {
    strip->setPixelColor(0, rVal, gVal, bVal);
    strip->show();
}

bool StateController :: is_connected(void) {
    return Bluefruit.connected();
}

void StateController :: load_settings(void) {
    settings = new SettingManager();
}

void StateController :: setup_rgbLED(void) {
    strip = new Adafruit_DotStar(1, 8, 6, DOTSTAR_BGR);
    int np = strip->numPixels();
    strip->begin();
    strip->setBrightness(13);
    strip->show();
}
