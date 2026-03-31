#ifndef STATE_H
#define STATE_H

#include "state_controller.h"

class State {
    public:
        virtual ~State(void) = default;
        virtual stateName get_name(void) = 0;
        virtual void enter(StateController* ctrl) = 0;
        virtual void exit(StateController* ctrl) = 0;
        virtual void update(StateController* ctrl) = 0;
};

#endif
