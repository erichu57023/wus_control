#ifndef BURSTING_STATE_H
#define BURSTING_STATE_H

#include "state.h"
#include "advertising_state.h"
#include "state_controller.h"
#include "nrf.h"
#include "nrf_gpio.h"

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
        uint8_t ctrlPort, counter;
        uint8_t dutyCycle = 50;
        uint32_t pulsePeriod, onDuration, timeout;
        uint64_t burstStartTime, pulseStartTime;

        // Methods
        void set_parameters(StateController* ctrl);
        void pulseOn(void);
        void pulseOff(void);
        uint32_t burst_elapsed(void);
        uint32_t pulse_elapsed(void);

};

#endif
