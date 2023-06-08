#ifndef STATE_CONTROLLER_H
#define STATE_CONTROLLER_H

#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <bluefruit.h>
#include "state.h"
#include "setting_manager.h"

class StateController {
    public:
        State* stateList[5];
        stateName currentState = NULL_STATE;
        stateName previousState = NULL_STATE;
        deviceStatus devStatus = DEVICE_NO_CONNECT;
        SettingManager* settings;
        String reprogramSetting = "";
        uint32_t reprogramValue = 0; 
        Adafruit_DotStar* strip;
        
        BLEUart bleuart;

        StateController(void);
        void add_state(State& state);
        void go_to_state(stateName name);
        void update(void);
        void set_rgbLED(uint8_t rVal, uint8_t gVal, uint8_t bVal);
        bool is_connected(void);
        
    private:
        void load_settings(void);
        void setup_rgbLED(void);
};
#include "setting_manager.h"

#endif
