#ifndef STATE_H
#define STATE_H

#include "state_controller.h"
#include <Arduino.h>

class State {
    public:
        stateName name = NULL_STATE;
        StateController* ctrl;

        void enter();
        void exit();
        void update();
};

#endif
