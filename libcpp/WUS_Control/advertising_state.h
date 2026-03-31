#ifndef ADVERTISING_STATE_H
#define ADVERTISING_STATE_H

#include "state.h"
#include "state_controller.h"
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

class AdvertisingState: virtual public State {
    private:
        BLEDis bledis;      // device information
        void setup_adv(void);
        void start_adv(void);

    public:
        StateController ctrl;
        AdvertisingState(StateController& controller);
        stateName get_name(void);
        void enter(void);
        void exit(void);
        void update(void);
};

#endif
