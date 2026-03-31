#include "state_controller.h"
#include "state.h"
#include "bursting_state.h"
#include "idle_state.h"
#include "advertising_state.h"
#include "programming_state.h"

StateController :: StateController(void) {
    this->load_settings();
    this->setup_rgbLED();
}

// Transition to a new state, calling the exit method of the current state and the enter method of the new state
void StateController :: go_to_state(stateName newStateName) {
    if (this->currentState != nullptr) {
        this->currentState->exit(this);
    }
    State* newState = nullptr;
    switch (newStateName) {
        case BURSTING_STATE:
            newState = &BurstingState::getInstance();
            break;
        case IDLE_STATE:
            newState = &IdleState::getInstance();
            break;
        case PROGRAMMING_STATE:
            newState = &ProgrammingState::getInstance();
            break;
        case ADVERTISING_STATE:
            newState = &AdvertisingState::getInstance();
            break;
        case NULL_STATE:
            break;
    }
    this->currentState = newState;
    this->currentState->enter(this);
}

// Return the name of the current state
stateName StateController :: current_state(void) {
    return this->currentState->get_name();
}

// Call the update method of the current state
void StateController :: update(void) {
    this->currentState->update(this);
}

// Set the DotStar RGB LED to the specified color
void StateController :: set_rgbLED(uint8_t rVal, uint8_t gVal, uint8_t bVal) {
    this->strip->setPixelColor(0, rVal, gVal, bVal);
    this->strip->show();
}

// Check if Bluetooth is currently connected
bool StateController :: is_connected(void) {
    return this->bf.connected(this->connection);
}

// Write to the Bursting Enabled characteristic to indicate whether bursting is currently on or off
void StateController :: report_bursting(bool burstOn) {
    this->reportEnabled.write8(burstOn);
}

// Load settings from the SettingManager singleton
void StateController :: load_settings(void) {
    this->settings = &SettingManager::getInstance();
}

// Initialize the DotStar RGB LED
void StateController :: setup_rgbLED(void) {
    this->strip = new Adafruit_DotStar(1, 8, 6, DOTSTAR_BGR);
    this->strip->begin();
    this->strip->setBrightness(1);
    this->strip->show();
}