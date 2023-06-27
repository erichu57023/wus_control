#include "state_controller.h"
#include "state.h"

StateController :: StateController(void) {
    this->load_settings();
    this->setup_rgbLED();
}

void StateController :: go_to_state(State& newState) {
    if (currentState) {
        previousState = currentState->get_name();
        currentState->exit(this);
    }
    currentState = &newState;
    currentState->enter(this);
}

stateName StateController :: current_state(void) {
    return currentState->get_name();
}

void StateController :: update(void) {
    currentState->update(this);
}

void StateController :: set_rgbLED(uint8_t rVal, uint8_t gVal, uint8_t bVal) {
    strip->setPixelColor(0, rVal, gVal, bVal);
    strip->show();
}

bool StateController :: is_connected(void) {
    return this->bf.connected(this->connection);
}

void StateController :: load_settings(void) {
    settings = &SettingManager::getInstance();
}

void StateController :: setup_rgbLED(void) {
    strip = new Adafruit_DotStar(1, 8, 6, DOTSTAR_BGR);
    int np = strip->numPixels();
    strip->begin();
    strip->setBrightness(13);
    strip->show();
}
