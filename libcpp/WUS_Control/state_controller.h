#ifndef STATE_CONTROLLER_H
#define STATE_CONTROLLER_H

#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <bluefruit.h>
#include "setting_manager.h"

enum stateName {NULL_STATE, ADVERTISING_STATE, BURSTING_STATE, IDLE_STATE, PROGRAMMING_STATE};
enum deviceStatus {DEVICE_OK, DEVICE_NO_CONNECT, DEVICE_INTERRUPT};
class State;
class StateController {
    public:
        // Singleton constructor methods
        static StateController& getInstance(void) {
            static StateController singleton;
            return singleton;
        }
        StateController(StateController const& other) = delete;
        StateController& operator = (const StateController& other) = delete;

        // Instance variables
        AdafruitBluefruit& bf = Bluefruit;
        volatile uint16_t connection;
        SettingManager* settings;
        Adafruit_DotStar* strip;
        BLEUart bleuart;
        volatile deviceStatus devStatus = DEVICE_NO_CONNECT;
        volatile mutableSetting reprogramSetting = NO_CHG;
        volatile uint32_t reprogramValue = 0; 

        // Methods
        void go_to_state(State& state);
        stateName current_state(void);
        void update(void);
        void set_rgbLED(uint8_t rVal, uint8_t gVal, uint8_t bVal);
        bool is_connected(void);
        
    private:
        // Constructor
        StateController(void);

        // Instance variables
        State* currentState = nullptr;
        stateName previousState = NULL_STATE;

        // Methods
        void load_settings(void);
        void setup_rgbLED(void);
};

#endif
