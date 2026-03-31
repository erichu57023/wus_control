#ifndef BURSTING_STATE_H
#define BURSTING_STATE_H

#include "state.h"
#include "advertising_state.h"
#include "state_controller.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include <nRF52_PWM.h>

class BurstingState: public State {
    public:
        // Singleton constructor methods
        static BurstingState& getInstance() {
            static BurstingState singleton;
            return singleton;
        }
        BurstingState(BurstingState const& other) = delete;
        BurstingState& operator = (const BurstingState& other) = delete;

        // Methods
        stateName get_name(void);
        void enter(StateController* ctrl);
        void exit(StateController* ctrl);
        void update(StateController* ctrl);

    private:
        // Constructor
        BurstingState(void) {};

        // Instance variables
        uint8_t ctrlPin;
        uint32_t timeout;
        unsigned long burstStartTime;
        float burstFreq;
        float dutyCycle = 100.0f;
        nRF52_PWM* burst_ctrl = nullptr;

        // Methods
        void set_parameters(StateController* ctrl);
        void pulseOn(void);
        void pulseOff(void);
        unsigned long burst_elapsed(void);
//        unsigned long pulse_elapsed(void);

};

#endif
