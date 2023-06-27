#ifndef STATE_H
#define STATE_H

#include "state_controller.h"

class State {
    public:
        virtual ~State(void) {};
        virtual stateName get_name(void);
        virtual void enter(StateController* ctrl);
        virtual void exit(StateController* ctrl);
        virtual void update(StateController* ctrl);
};

#endif
