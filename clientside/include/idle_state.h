#ifndef IDLE_STATE_H
#define IDLE_STATE_H

#include "state.h"

class IdleState: public State {
    public:
        // Singleton constructor methods
        static IdleState& getInstance() {
            static IdleState singleton;
            return singleton;
        }
        IdleState(IdleState const& other) = delete;
        IdleState& operator = (const IdleState& other) = delete;

        // Methods
        stateName get_name(void);
        void enter(StateController* ctrl);
        void exit(StateController* ctrl);
        void update(StateController* ctrl);

    private:
        // Constructor
        IdleState(void) {};
};

#endif
