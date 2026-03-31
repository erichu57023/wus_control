#include "advertising_state.h"

stateName AdvertisingState :: get_name(void) {
    return ADVERTISING_STATE;
}

void AdvertisingState :: enter(StateController* ctrl) {
    ctrl->set_rgbLED(255, 0, 0); // red
    this->start_adv(ctrl);
    // Serial.println("advertising!");
}

void AdvertisingState :: exit(StateController* ctrl) {
    // ctrl->bf.Advertising.stop();
}

void AdvertisingState :: update(StateController* ctrl) {
    if (ctrl->is_connected()) {
        ctrl->devStatus = DEVICE_OK;
        // ctrl->go_to_state(ProgrammingState::getInstance());
    }
}

void AdvertisingState :: start_adv(StateController* ctrl) {
    ctrl->bf.autoConnLed(false);
    ctrl->bf.configPrphBandwidth(BANDWIDTH_MAX);

    ctrl->bf.begin();
    ctrl->bf.setTxPower(0);    // Check bluefruit.h for supported values
    ctrl->bf.Periph.setConnectCallback(this->connect_callback);
    ctrl->bf.Periph.setDisconnectCallback(this->disconnect_callback);

    this->bledfu.begin();

    // Configure and Start Device Information Service
    this->bledis.setManufacturer("Adafruit Industries");
    this->bledis.setModel("IB-nRF52840-X");
    this->bledis.begin();

    // Configure and Start BLE Uart Service
    ctrl->bleuart.begin();

    // Start BLE Battery Service
    this->blebas.begin();
    this->blebas.write(100);

    // Advertising packet
    ctrl->bf.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    ctrl->bf.Advertising.addTxPower();

    // Include bleuart 128-bit uuid
    ctrl->bf.Advertising.addService(ctrl->bleuart);
    ctrl->bf.ScanResponse.addName();

    ctrl->bf.Advertising.restartOnDisconnect(1);
    ctrl->bf.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    ctrl->bf.Advertising.setFastTimeout(30);      // number of seconds in fast mode
    ctrl->bf.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void AdvertisingState :: connect_callback(uint16_t conn_handle) {
    // Get the reference to current connection
    StateController* ctrl = &StateController::getInstance();
    ctrl->connection = conn_handle;
    // Serial.println(conn_handle);
}

void AdvertisingState :: disconnect_callback(uint16_t conn_handle, uint8_t reason) {
    (void) conn_handle;
    (void) reason;

    StateController* ctrl = &StateController::getInstance();
    ctrl->go_to_state(AdvertisingState::getInstance());
}
