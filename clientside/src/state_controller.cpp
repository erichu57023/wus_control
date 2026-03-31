#include "state_controller.h"
#include "state.h"

StateController :: StateController(void) {
    this->load_settings();
    this->setup_rgbLED();
}

void StateController :: go_to_state(State& newState) {
    if (this->currentState) {
        // previousState = currentState->get_name();
        this->currentState->exit(this);
    }
    this->currentState = &newState;
    this->currentState->enter(this);
}

stateName StateController :: current_state(void) {
    return this->currentState->get_name();
}

void StateController :: update(void) {
    this->currentState->update(this);
}

void StateController :: set_rgbLED(uint8_t rVal, uint8_t gVal, uint8_t bVal) {
    this->strip->setPixelColor(0, rVal, gVal, bVal);
    this->strip->show();
}

bool StateController :: is_connected(void) {
    return this->bf.connected(this->connection);
}

void StateController :: report_bursting(bool burstOn) {
    this->reportEnabled.write8(burstOn);
}

void StateController :: load_settings(void) {
    this->settings = &SettingManager::getInstance();
}

void StateController :: setup_rgbLED(void) {
    this->strip = new Adafruit_DotStar(1, 8, 6, DOTSTAR_BGR);
    this->strip->begin();
    this->strip->setBrightness(1);
    this->strip->show();
}