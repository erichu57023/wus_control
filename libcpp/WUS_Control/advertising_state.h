#ifndef ADVERTISING_STATE_H
#define ADVERTISING_STATE_H

#include "state.h"
#include <Arduino.h>
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

class AdvertisingState: public State {
    private:
        #define RED_LED 3

        BLEDis bledis;      // device information
        BLEUart bleuart;    // uart over ble
        BLEBas blebas;      // battery

        void setup_adv();
        void start_adv();

    public:
        stateName name = ADVERTISING_STATE;
        StateController* ctrl;
        
        AdvertisingState(StateController* controller);
        void enter();
        void exit();
        void update();
};

#endif
