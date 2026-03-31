#ifndef PROGRAMMING_STATE_H
#define PROGRAMMING_STATE_H

#include "state.h"

// Forward declarations
class StateController; // Needed for compilation
enum stateName : uint8_t;

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
};

#endif
