#ifndef BURSTING_STATE_H
#define BURSTING_STATE_H

#include "state.h"
#include "setting_manager.h"
#include <nrfx_pwm.h>
extern "C" {
    #include <hal/nrf_timer.h>
    #include <hal/nrf_gpiote.h>
    #include <hal/nrf_ppi.h>
}

// Forward declarations
class StateController; // Needed for compilation
enum stateName : uint8_t;

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
        
        static void pulseOn(void);
        static void pulseOff(void);

        static void initialize(void);
        static void program_PWM(void);
        static void program_stim_timer(void);
        static void program_timeout_timer(void);
        static void program_event_hooks(void);

    private:
        // Constructor
        BurstingState(void) {};

        // Instance variables
        static nrf_timer_cc_channel_t stimOffClock, stimOnClock, timeoutClock;
        static nrf_pwm_values_common_t compareSeq[256];
        static nrf_ppi_channel_t onHook, offHook;
        static constexpr bool centeredPWM = 0;
        static constexpr uint32_t burstPin = nrf52840_port_map[SettingManager::cs_burst_control];

        // Methods
        static void program_simple_PWM();
        static void program_arbitrary_PWM();

};

extern "C" {
    void TIMER2_IRQHandler(void);
}

#endif
