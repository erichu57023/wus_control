#ifndef PROGRAMMING_STATE_H
#define PROGRAMMING_STATE_H

#include "state_controller.h"
#include "state.h"
#include "advertising_state.h"
#include "interrupt_state.h"
#include "idle_state.h"

class ProgrammingState: public State {
    public:
        // Singleton constructor methods
        static ProgrammingState& getInstance() {
            static ProgrammingState singleton;
            return singleton;
        }
        ProgrammingState(ProgrammingState const& other) = delete;
        ProgrammingState& operator = (const ProgrammingState& other) = delete;

        // Methods
        stateName get_name(void);
        void enter(StateController* ctrl);
        void exit(StateController* ctrl);
        void update(StateController* ctrl);

    private:
        // Constructor
        ProgrammingState(void) {};

        // Instance variables
        bool initialized = false;

        // Methods
        void startup_sequence(StateController* ctrl);
        void change_setting(StateController* ctrl);
        // void gpio_clock_8m(uint8_t pin);
};

#endif
