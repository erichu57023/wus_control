#ifndef INTERRUPT_STATE_H
#define INTERRUPT_STATE_H

#include "state.h"
#include "advertising_state.h"
#include "bursting_state.h"
#include "state_controller.h"
#include <ctype.h>
#include <string.h>

#define BUFFER_LEN 64
static char readBuffer[BUFFER_LEN];
static bool str_equals(const char* strA, const char* strB);

class InterruptState: public State {
    public:
        // Singleton constructor methods
        static InterruptState& getInstance() {
            static InterruptState singleton;
            return singleton;
        }
        InterruptState(InterruptState const& other) = delete;
        InterruptState& operator = (const InterruptState& other) = delete;

        // Methods
        stateName get_name(void);
        void enter(StateController* ctrl);
        void exit(StateController* ctrl);
        void update(StateController* ctrl);

    private:
        // Constructor
        InterruptState(void) {};

        // Methods
        void read_input_buffer(StateController* ctrl);
        void parse_command(StateController* ctrl);
};

#endif
