#ifndef STATE_CONTROLLER_H
#define STATE_CONTROLLER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_DotStar.h>
#include <bluefruit.h>

enum stateName {NULL_STATE, ADVERTISING_STATE, BURSTING_STATE, IDLE_STATE, PROGRAMMING_STATE};
enum deviceStatus {DEVICE_OK, DEVICE_NO_CONNECT, DEVICE_INTERRUPT};

class State;
class SettingManager;
class StateController {
    public:
        State* currentState;
        State* stateList[5];
        stateName previousState;
        SettingManager* settings;
        String reprogramSetting;
        BLEUart* bleuart;
        deviceStatus devStatus;

        StateController();
        void add_state(State* state);
        void go_to_state(stateName name);
        void update();
        void set_rgbLED(int rVal, int gVal, int bVal);
        bool is_connected();
        
    private:
        Adafruit_DotStar* rgbLED;
        void load_settings();
        void setup_rgbLED();
};
#include "setting_manager.h"
#include "state.h"

#endif
