#ifndef PROGRAMMING_STATE_H
#define PROGRAMMING_STATE_H

#include "state.h"
#include "state_controller.h"
#include <nRF52_PWM.h>
#include "AD9833.h"
#include "TUSS4470.h"

class ProgrammingState: virtual public State {
    public:
        StateController ctrl;
        ProgrammingState(StateController& controller);
        stateName get_name(void);
        void enter(void);
        void exit(void);
        void update(void);

    private:
        nRF52_PWM* wavePWM;
        AD9833* waveGen;
        TUSS4470* burstGen;

        void startup_sequence(void);
        void change_setting(void);
};

#endif
