#include "bursting_state.h"
#include "state_controller.h"
#include "advertising_state.h"

/* Initialize static variables */
nrf_timer_cc_channel_t BurstingState :: stimOffClock   = NRF_TIMER_CC_CHANNEL0; // In Timer1
nrf_timer_cc_channel_t BurstingState :: stimOnClock    = NRF_TIMER_CC_CHANNEL1; // In Timer1
nrf_timer_cc_channel_t BurstingState :: timeoutClock   = NRF_TIMER_CC_CHANNEL0; // In Timer2
nrf_pwm_values_common_t BurstingState :: compareSeq[256];
nrf_ppi_channel_t BurstingState :: onHook  = NRF_PPI_CHANNEL0;
nrf_ppi_channel_t BurstingState :: offHook = NRF_PPI_CHANNEL1;

/* ---------------------------------------------------- PUBLIC ----------------------------------------------------*/

// Return state name
stateName BurstingState :: get_name(void) {
    return BURSTING_STATE;
}

// Set burst generator to active mode and turn on stimulation
void BurstingState :: enter(StateController* ctrl) {
    ctrl->set_rgbLED(0, 0, 255); // blue
    ctrl->burstGen->disableStandbyMode();
    ctrl->report_bursting(true);
    this->pulseOn();
}

// Turn off stimulation and put burst generator in standby mode
void BurstingState :: exit(StateController* ctrl) {
    this->pulseOff();
    ctrl->burstGen->enableStandbyMode();
    ctrl->report_bursting(false);
}

// Remain in bursting state until an interrupt triggers exit
void BurstingState :: update(StateController* ctrl) {}

// Turn on stimulation
void BurstingState :: pulseOn(void) {
    nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_CLEAR);   // Clear stim timer
    nrf_timer_task_trigger(NRF_TIMER2, NRF_TIMER_TASK_CLEAR);   // Clear timeout timer
    
    nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_START);   // Start stim timer
    nrf_timer_task_trigger(NRF_TIMER2, NRF_TIMER_TASK_START);   // Start timeout timer

    nrf_pwm_task_trigger(NRF_PWM0, NRF_PWM_TASK_SEQSTART0);     // Trigger PWM sequence
    nrf_gpiote_task_trigger(NRF_GPIOTE, NRF_GPIOTE_TASK_SET_0); // Turn LED on (needs to be through GPIOTE)
}

// Turn off stimulation
void BurstingState :: pulseOff(void) {
    nrf_timer_task_trigger(NRF_TIMER1, NRF_TIMER_TASK_STOP);    // Stop stim timer
    nrf_timer_task_trigger(NRF_TIMER2, NRF_TIMER_TASK_STOP);    // Stop timeout timer

    nrf_pwm_task_trigger(NRF_PWM0, NRF_PWM_TASK_STOP);          // Stop PWM sequence
    nrf_gpiote_task_trigger(NRF_GPIOTE, NRF_GPIOTE_TASK_CLR_0); // Turn LED off (needs to be through GPIOTE)
}

/* Initialize all settings (from ProgrammingState) only once */
void BurstingState :: initialize(void) {
    // Set up LED pin for status indication
    nrf_gpio_cfg_output(LED_PIN);       // Configure led pin as output
    nrf_gpio_pin_clear(LED_PIN);        // Set default pin value as low

    // Set up burst pin for PWM output
    nrf_gpio_cfg_output(burstPin);      // Configure burst pin as output
    nrf_gpio_pin_set(burstPin);         // Set default pin value as high
    nrf_pwm_enable(NRF_PWM0);
    program_PWM();
    uint32_t output_pins[4] = {burstPin, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED, NRF_PWM_PIN_NOT_CONNECTED};
    nrf_pwm_pins_set(NRF_PWM0, output_pins);
    nrf_pwm_decoder_set(NRF_PWM0, NRF_PWM_LOAD_COMMON, NRF_PWM_STEP_AUTO);
    nrf_pwm_shorts_enable(NRF_PWM0, NRF_PWM_SHORT_LOOPSDONE_SEQSTART0_MASK);
    nrf_pwm_loop_set(NRF_PWM0, 0xFFFF); // Loop indefinitely

    // Set up stim timers for pulse timing
    program_stim_timer();
    nrf_timer_shorts_enable(NRF_TIMER1, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK); // Clear on full period
    nrf_timer_int_disable(NRF_TIMER1, NRF_TIMER_INT_COMPARE0_MASK | NRF_TIMER_INT_COMPARE1_MASK); // Disable interrupts
    
    // Set up timeout timer for automatic return to idle state after stimulation
    program_timeout_timer();
    nrf_timer_shorts_enable(NRF_TIMER2, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK); // Clear on timeout
    nrf_timer_int_enable(NRF_TIMER2, NRF_TIMER_INT_COMPARE0_MASK); // Enable timeout interrupt
    NVIC_SetPriority(TIMER2_IRQn, 7); // Set lowest priority to avoid interrupt conflicts
    NVIC_EnableIRQ(TIMER2_IRQn);

    // Set up PPI hooks for automatic triggering of events
    program_event_hooks();   
}

// Set up PWM based on whether a DC sequence is specified by the user
void BurstingState :: program_PWM(void) {
    if (SettingManager::dc_seq_len == 1) {
        program_simple_PWM();
    } else {
        program_arbitrary_PWM();
    }
}

// Create a hi-frequency timer (TIMER1) to trigger stim ON/OFF events
void BurstingState :: program_stim_timer(void) {
    // Calculate timer resolution
    nrf_timer_frequency_t prescaler = static_cast<nrf_timer_frequency_t>(constrain(ceil(log(16e6 * SettingManager::stim_pd / 1e6) / log(2) - 24), 0, 9));

    // Initialize clock
    nrf_timer_frequency_set(NRF_TIMER1, prescaler);
    nrf_timer_mode_set(NRF_TIMER1, NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(NRF_TIMER1, NRF_TIMER_BIT_WIDTH_24);
    
    // Calculate compare register values for stim ON/OFF events
    uint32_t on_over  = nrf_timer_us_to_ticks(round(SettingManager::stim_pd * SettingManager::stim_dc / 100.0f), prescaler);
    uint32_t off_over = nrf_timer_us_to_ticks(round(SettingManager::stim_pd), prescaler);

    // Set compare register values
    nrf_timer_cc_set(NRF_TIMER1, stimOffClock, on_over);
    nrf_timer_cc_set(NRF_TIMER1, stimOnClock, off_over);
}

// Create a low-frequency timer (TIMER2)to trigger timeout event
void BurstingState :: program_timeout_timer(void) {
    // Calculate timer resolution
    nrf_timer_frequency_t prescaler = static_cast<nrf_timer_frequency_t>(constrain(ceil(log(16e6 * SettingManager::timeout / 1e3) / log(2) - 24), 0, 9));
    
    // Initialize clock
    nrf_timer_frequency_set(NRF_TIMER2, prescaler);
    nrf_timer_mode_set(NRF_TIMER2, NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(NRF_TIMER2, NRF_TIMER_BIT_WIDTH_24);

    // Calculate compare register values for timeout event
    uint32_t timeout_over = nrf_timer_ms_to_ticks(round(SettingManager::timeout), prescaler);

    // Set compare register values
    nrf_timer_cc_set(NRF_TIMER2, timeoutClock, timeout_over);
}

// Use PPI to hook events to tasks for automatic triggering. Only needs to be run once.
void BurstingState :: program_event_hooks(void) {
    // Initialize GPIOTE to trigger LED pin automatically
    // Using nrf HAL instead of nrfx due to linker errors between nrfx and arduino
    nrf_gpiote_task_configure(
        NRF_GPIOTE,
        0, 
        LED_PIN,
        NRF_GPIOTE_POLARITY_TOGGLE,
        NRF_GPIOTE_INITIAL_VALUE_LOW
    );
    nrf_gpiote_task_enable(NRF_GPIOTE, 0);

    // When stim reaches full period, turn on PWM, LED ON
    nrf_ppi_channel_and_fork_endpoint_setup(NRF_PPI, onHook, 
            nrf_timer_event_address_get(NRF_TIMER1, NRF_TIMER_EVENT_COMPARE1),        // Full period
            nrf_pwm_task_address_get(NRF_PWM0, NRF_PWM_TASK_SEQSTART0),               // Start PWM
            nrf_gpiote_task_address_get(NRF_GPIOTE, NRF_GPIOTE_TASK_SET_0));          // LED ON

    // When stim reaches end of duty period, turn off PWM, LED OFF
    nrf_ppi_channel_and_fork_endpoint_setup(NRF_PPI, offHook, 
            nrf_timer_event_address_get(NRF_TIMER1, NRF_TIMER_EVENT_COMPARE0),        // Full period
            nrf_pwm_task_address_get(NRF_PWM0, NRF_PWM_TASK_STOP),                    // Stop PWM
            nrf_gpiote_task_address_get(NRF_GPIOTE, NRF_GPIOTE_TASK_CLR_0));          // LED OFF
 
    // Enable hooks
    nrf_ppi_channel_enable(NRF_PPI, onHook);
    nrf_ppi_channel_enable(NRF_PPI, offHook);
}

/* ---------------------------------------------------- PRIVATE ---------------------------------------------------*/

/*  Set up a constant duty cycle PWM output. */
void BurstingState :: program_simple_PWM(void) {
    // Calculate PWM register values
    float countTop = 16e6 * SettingManager::burst_pd;
    uint8_t prescaler = 0;
    while (countTop > 0x7fff && prescaler < 7) { // countTop is a 15-bit register
        prescaler++;
        countTop /= 2;
    }
    countTop = round(countTop);

    // NEEDS to be static (not stored on the stack)
    static nrf_pwm_values_common_t compVal = round(countTop * SettingManager::burst_dc / 100.0f);
    compVal |= 0x8000; // Set MSB to enable output inversion (active low)

    // Set PWM register values
    nrf_pwm_configure(NRF_PWM0, static_cast<nrf_pwm_clk_t>(prescaler), 
                      NRF_PWM_MODE_UP, static_cast<uint16_t>(countTop));

    // Define a duty cycle sequence
    nrf_pwm_sequence_t seq0;
    seq0.length = 1;
    seq0.values.p_common = &compVal;
    seq0.end_delay = 0;
    seq0.repeats = 0;

    // Set both sequences to allow looping shortcut
    nrf_pwm_sequence_set(NRF_PWM0, 0, &seq0);
    nrf_pwm_sequence_set(NRF_PWM0, 1, &seq0);
}


/*  Set up an arbitrary duty cycle sequence playback for the pulse ON time, 
    with a delay for the pulse OFF time. The sequence must end with 0.  */
void BurstingState :: program_arbitrary_PWM(void) {
    // Calculate PWM register values
    float countTop = round(16e6f * SettingManager::burst_pd * SettingManager::burst_dc / 100.0f
                              / SettingManager::dc_seq_len / (centeredPWM + 1));
    uint8_t prescaler = 0;
    while (countTop > 0x7fff && prescaler < 7) { // countTop is a 15-bit register
        prescaler++;
        countTop /= 2;
    }
    countTop = round(countTop);

    // NEEDS to be static (not stored on the stack)
    for (unsigned int i = 0; i < SettingManager::dc_seq_len; i++) {
        compareSeq[i] = round(SettingManager::dc_seq_vals[i] / 255.0f * countTop);
        compareSeq[i] |= 0x8000; // Set MSB to enable output inversion (active low)
    } 
    uint32_t off_time_loops = round(SettingManager::dc_seq_len * (100.0f / SettingManager::burst_dc - 1.0f));
    
    // Set PWM register values
    nrf_pwm_configure(NRF_PWM0, static_cast<nrf_pwm_clk_t>(prescaler), 
                      static_cast<nrf_pwm_mode_t>(centeredPWM), static_cast<uint16_t>(countTop));
    
    // Define a duty cycle sequence
    nrf_pwm_sequence_t seq0;
    seq0.length = SettingManager::dc_seq_len;
    seq0.values.p_common = compareSeq;
    seq0.end_delay = off_time_loops;    // For this to work, the final value in the sequence must be 0.
    seq0.repeats = 0;

    // Set both sequences to allow looping shortcut
    nrf_pwm_sequence_set(NRF_PWM0, 0, &seq0);
    nrf_pwm_sequence_set(NRF_PWM0, 1, &seq0);
}


// Interrupt when the timeout timer triggers, stop bursting, return to IdleState
extern "C" {
    void TIMER2_IRQHandler(void) {
        if (nrf_timer_event_check(NRF_TIMER2, NRF_TIMER_EVENT_COMPARE0)) {
            nrf_timer_event_clear(NRF_TIMER2, NRF_TIMER_EVENT_COMPARE0);
            StateController* ctrl = &StateController::getInstance();
            ctrl->go_to_state(IDLE_STATE);
        }
    }
}