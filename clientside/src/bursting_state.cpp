#include "bursting_state.h"

/* Initialize static variables */
nrfx_pwm_t   BurstingState :: pwm_driver   = NRFX_PWM_INSTANCE(0);
nrfx_timer_t BurstingState :: burstClock   = NRFX_TIMER_INSTANCE(1);
nrfx_rtc_t   BurstingState :: timeoutClock = NRFX_RTC_INSTANCE(2);
uint32_t*    BurstingState :: pwmTrigger;
nrf_pwm_values_common_t BurstingState :: compareSeq[256];
nrf_ppi_channel_t BurstingState :: onHook;
nrf_ppi_channel_t BurstingState :: offHook;

/* ---------------------------------------------------- PUBLIC ----------------------------------------------------*/

stateName BurstingState :: get_name(void) {
    return BURSTING_STATE;
}

void BurstingState :: enter(StateController* ctrl) {
    ctrl->set_rgbLED(0, 0, 255); // blue
    ctrl->burstGen->disableStandbyMode();
    ctrl->report_bursting(true);
    this->pulseOn();
}

void BurstingState :: exit(StateController* ctrl) {
    this->pulseOff();
    ctrl->burstGen->enableStandbyMode();
    ctrl->report_bursting(false);
}

// Remain in bursting state until an interrupt triggers exit
void BurstingState :: update(StateController* ctrl) {}

// Turn on stimulation
void BurstingState :: pulseOn(void) {
    (void) nrfx_timer_clear(&burstClock);         // Clear stim timer
    (void) nrfx_rtc_counter_clear(&timeoutClock); // Clear timeout timer
    
    (void) nrfx_timer_enable(&burstClock);        // Start stim timer
    (void) nrfx_rtc_enable(&timeoutClock);        // Start timeout timer

    *this->pwmTrigger = 1;                        // Trigger PWM sequence
    nrf_gpiote_task_trigger(NRF_GPIOTE, NRF_GPIOTE_TASK_SET_0);       // Turn LED on (needs to be through GPIOTE)
}

// Turn off stimulation
void BurstingState :: pulseOff(void) {
    (void) nrfx_timer_disable(&burstClock);       // Disable stim timer
    (void) nrfx_rtc_disable(&timeoutClock);       // Disable timeout timer

    (void) nrfx_pwm_stop(&this->pwm_driver, false);     // Stop PWM sequence
    nrf_gpiote_task_trigger(NRF_GPIOTE, NRF_GPIOTE_TASK_CLR_0);       // Turn LED off (needs to be through GPIOTE)
}

/* Initialize all settings (from ProgrammingState) */
void BurstingState :: initialize(void) {
    nrf_gpio_cfg_output(LED_PIN);       // Configure led pin as output
    nrf_gpio_pin_set(LED_PIN);          // Set default pin value as high

    program_PWM();
    program_burst_timer();
    program_timeout_timer();
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

// Create a hi-frequency timer to trigger stim ON/OFF events
void BurstingState :: program_burst_timer(void) {
    // Calculate timer resolution
    uint8_t prescaler = constrain(ceil(log(16e6 * SettingManager::stim_pd / 1e6) / log(2) - 24), 0, 9);

    // Initialize clock
    nrfx_timer_config_t burst_config;
    burst_config.frequency = static_cast<nrf_timer_frequency_t>(prescaler);
    burst_config.mode = NRF_TIMER_MODE_TIMER;
    burst_config.bit_width = NRF_TIMER_BIT_WIDTH_24;
    burst_config.interrupt_priority = 8;
    (void) nrfx_timer_init(&burstClock, &burst_config, (nrfx_timer_event_handler_t) nrfx_timer_1_irq_handler);
    
    // Calculate compare register values for stim ON/OFF events
    uint32_t on_over  = nrfx_timer_us_to_ticks(&burstClock, round(SettingManager::stim_pd * SettingManager::stim_dc));
    uint32_t off_over = nrfx_timer_us_to_ticks(&burstClock, round(SettingManager::stim_pd));

    // Set compare register values; shortcut timer to auto-clear after second event
    (void) nrfx_timer_compare(&burstClock, NRF_TIMER_CC_CHANNEL0, on_over, false);
    (void) nrfx_timer_extended_compare(&burstClock, NRF_TIMER_CC_CHANNEL1, off_over, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, false);
}

// Create a low-frequency timer to trigger timeout event
void BurstingState :: program_timeout_timer(void) {
    // Calculate timer resolution
    uint16_t prescaler = floor(SettingManager::timeout / 512e6f);
    float counterFreq = 32768.0f / (prescaler + 1);
    
    // Initialize clock
    nrfx_rtc_config_t timeout_config;
    timeout_config.prescaler = prescaler;
    timeout_config.interrupt_priority = 8;
    timeout_config.tick_latency = (uint8_t) round(2e-3 * counterFreq);
    timeout_config.reliable = false;
    (void) nrfx_rtc_init(&timeoutClock, &timeout_config, (nrfx_rtc_handler_t) irq_handler_RTC);
  
    // Set compare register values
    uint32_t timeout_over = static_cast<uint32_t>(NRFX_RTC_US_TO_TICKS(SettingManager::timeout, counterFreq));
    (void) nrfx_rtc_tick_disable(&timeoutClock);      // Not needed, turn off to save power
    (void) nrfx_rtc_cc_set(&timeoutClock, 0, timeout_over, true);             // Set compare register
}

// Use PPI to hook events to tasks for automatic triggering. Only needs to be run once.
void BurstingState :: program_event_hooks(void) {
    // Allocate available PPI channels
    (void) nrfx_ppi_channel_alloc(&onHook);
    (void) nrfx_ppi_channel_alloc(&offHook);

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
    (void) nrfx_ppi_channel_assign(onHook,
                nrfx_timer_event_address_get(&burstClock, NRF_TIMER_EVENT_COMPARE1),      // Full period
                nrfx_pwm_task_address_get(&pwm_driver, NRF_PWM_TASK_SEQSTART0));          // Start PWM
    (void) nrfx_ppi_channel_fork_assign(onHook,
                nrf_gpiote_task_address_get(NRF_GPIOTE, NRF_GPIOTE_TASK_SET_0));          // LED ON

    // When stim reaches end of duty period, turn off PWM, LED OFF
    (void) nrfx_ppi_channel_assign(offHook,
                nrfx_timer_event_address_get(&burstClock, NRF_TIMER_EVENT_COMPARE0),      // Duty period
                nrfx_pwm_task_address_get(&pwm_driver, NRF_PWM_TASK_STOP));               // Stop PWM
    (void) nrfx_ppi_channel_fork_assign(offHook, 
                nrf_gpiote_task_address_get(NRF_GPIOTE, NRF_GPIOTE_TASK_CLR_0));          // LED OFF    

    // Enable hooks
    (void) nrfx_ppi_channel_enable(onHook);
    (void) nrfx_ppi_channel_enable(offHook);
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

    // NEEDS to be static (stored in heap memory, not stack)
    static nrf_pwm_values_common_t compVal = round(countTop * SettingManager::burst_dc);

    // Set PWM register values
    static nrfx_pwm_config_t pwm_config_simple;
    pwm_config_simple.output_pins[0] = SettingManager::cs_burst_control + NRFX_PWM_PIN_INVERTED;
    pwm_config_simple.irq_priority = 8;
    pwm_config_simple.base_clock = static_cast<nrf_pwm_clk_t>(prescaler);
    pwm_config_simple.count_mode = NRF_PWM_MODE_UP;
    pwm_config_simple.top_value = static_cast<uint16_t>(countTop);
    pwm_config_simple.load_mode = NRF_PWM_LOAD_COMMON;
    pwm_config_simple.step_mode = NRF_PWM_STEP_AUTO;
    pwm_config_simple.skip_gpio_cfg = true;
    (void) nrfx_pwm_init(&pwm_driver, &pwm_config_simple, NULL, NULL);

    // Define a duty cycle sequence
    nrf_pwm_sequence_t seq0;
    seq0.length = 1;
    seq0.values.p_common = &compVal;
    seq0.end_delay = 0;    // For this to work, the final value in the sequence must be 0.
    seq0.repeats = 0;

    // Create a PWM task and store a trigger address pointer to be activated later
    uintptr_t triggerAddress = (uintptr_t) nrfx_pwm_simple_playback(&pwm_driver, &seq0,
                                    0xFFFF, NRFX_PWM_FLAG_LOOP + NRFX_PWM_FLAG_START_VIA_TASK);
    pwmTrigger = (uint32_t*) triggerAddress;
}


/*  Set up an arbitrary duty cycle sequence playback for the pulse ON time, 
    with a delay for the pulse OFF time. The sequence must end with 0.  */
void BurstingState :: program_arbitrary_PWM(void) {
    // Calculate PWM register values
    double countTop = round(16e6f * SettingManager::burst_pd * SettingManager::burst_dc
                              / SettingManager::dc_seq_len / (centeredPWM + 1));
    uint8_t prescaler = 0;
    while (countTop > 0x7fff && prescaler < 7) { // countTop is a 15-bit register
        prescaler++;
        countTop /= 2;
    }
    countTop = round(countTop);

    for (unsigned int i = 0; i < SettingManager::dc_seq_len; i++) {
        compareSeq[i] = round(SettingManager::dc_seq_vals[i] / 255.0f * countTop);
    }
    float off_time_loops = round(SettingManager::dc_seq_len * (1.0f / SettingManager::burst_dc - 1.0f));
    
    // Set PWM register values
    static nrfx_pwm_config_t pwm_config_arbitrary;
    pwm_config_arbitrary.output_pins[0] = SettingManager::cs_burst_control + NRFX_PWM_PIN_INVERTED;
    pwm_config_arbitrary.irq_priority = 8;
    pwm_config_arbitrary.base_clock = static_cast<nrf_pwm_clk_t>(prescaler);
    pwm_config_arbitrary.count_mode = static_cast<nrf_pwm_mode_t>(centeredPWM);
    pwm_config_arbitrary.top_value = static_cast<uint16_t>(round(countTop));
    pwm_config_arbitrary.load_mode = NRF_PWM_LOAD_COMMON;
    pwm_config_arbitrary.step_mode = NRF_PWM_STEP_AUTO;
    pwm_config_arbitrary.skip_gpio_cfg = true;
    (void) nrfx_pwm_init(&pwm_driver, &pwm_config_arbitrary, NULL, NULL);

    // Define a duty cycle sequence
    nrf_pwm_sequence_t seq0;
    seq0.length = SettingManager::dc_seq_len;
    seq0.values.p_common = compareSeq;
    seq0.end_delay = off_time_loops;    // For this to work, the final value in the sequence must be 0.
    seq0.repeats = 0;

    // Create a PWM task and store a trigger address pointer to be activated later
    uintptr_t triggerAddress = (uintptr_t) nrfx_pwm_simple_playback(&pwm_driver, &seq0,
                                    0xFFFF, NRFX_PWM_FLAG_LOOP + NRFX_PWM_FLAG_START_VIA_TASK);
    pwmTrigger = (uint32_t*) triggerAddress;
}

// Interrupt when the timeout timer triggers to stop bursting
void BurstingState :: irq_handler_RTC(nrfx_rtc_int_type_t int_type) {
    if (int_type == NRFX_RTC_INT_COMPARE0) {
        BurstingState* dev = &BurstingState::getInstance();
        dev->pulseOff();
    }
}