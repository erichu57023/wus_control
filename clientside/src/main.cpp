#include <Arduino.h>
#include "state_controller.h"
#include <hal/nrf_qspi.h>

// See "DEFAULT_SETTINGS.h" for a list of valid settings

StateController* device = &StateController::getInstance();

static void disable_unused_peripherals(void) {
    // Disable QSPI flash to save power
    nrf_qspi_cinstr_conf_t qspi_flash_off_config = {
        .opcode    = 0xB9,                // Deep Power-Down Opcode
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = false,
        .io3_level = false,
        .wipwait   = false,
        .wren      = false
    };

    nrf_qspi_task_trigger(NRF_QSPI, NRF_QSPI_TASK_ACTIVATE); // Activate QSPI interface
    while (!(nrf_qspi_event_check(NRF_QSPI, NRF_QSPI_EVENT_READY))) {} // Wait until QSPI is ready before sending instruction
    nrf_qspi_cinstr_transfer_start(NRF_QSPI, &qspi_flash_off_config); // Put external flash in deep power-down mode to save power
    nrf_qspi_task_trigger(NRF_QSPI, NRF_QSPI_TASK_DEACTIVATE); // Deactivate QSPI interface to save power

    (void) sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE); // Enable DCDC converter for lower power consumption
}

void setup() {
    // Serial.begin(115200);
    Serial.end();
    disable_unused_peripherals();
    device->go_to_state(PROGRAMMING_STATE);
}

void loop() {
    device->update();
}
