#ifndef BURSTING_STATE_H
#define BURSTING_STATE_H

#include "state.h"
#include "advertising_state.h"
#include "state_controller.h"
#include "setting_manager.h"
#include <nrfx_pwm.h>
#include <nrfx_rtc.h>
#include <nrfx_timer.h>
#include <nrfx_ppi.h>
extern "C" {
    #include <hal/nrf_gpiote.h>
}

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
        
        void pulseOn(void);
        void pulseOff(void);

        static void initialize(void);
        static void program_PWM(void);
        static void program_burst_timer(void);
        static void program_timeout_timer(void);
        static void program_event_hooks(void);

    private:
        // Constructor
        BurstingState(void) {};

        // Instance variables
        static nrfx_pwm_t pwm_driver;
        static nrfx_timer_t burstClock;
        static nrfx_rtc_t timeoutClock;
        static uint32_t* pwmTrigger;
        static nrf_pwm_values_common_t compareSeq[256];
        static nrf_ppi_channel_t onHook, offHook;
        static constexpr bool centeredPWM = 0;

        // Methods
        static void program_simple_PWM();
        static void program_arbitrary_PWM();
        static void irq_handler_RTC(nrfx_rtc_int_type_t int_type);


};

#endif
