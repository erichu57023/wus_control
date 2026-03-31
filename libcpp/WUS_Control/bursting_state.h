#ifndef BURSTING_STATE_H
#define BURSTING_STATE_H

#define LED_PIN 6

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
        uint8_t ctrlPin;
        uint32_t timeout;
        bool burstActive;
        unsigned long burstStartTime, stimStartTime, stimOnTime;
        unsigned long stimPeriod = 1e6;
        float burstFreq = 0.0f;
        float burstDC = 100.0f;
        float stimDC = 100.0f;
        uint32_t dutyCycleComp = 0;

        // Methods
        void set_parameters(StateController* ctrl);
        void pulseOn(void);
        void pulseOff(void);

};

#endif
