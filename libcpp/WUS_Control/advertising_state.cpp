#include "advertising_state.h"

stateName AdvertisingState :: get_name(void) {
    return ADVERTISING_STATE;
}

void AdvertisingState :: enter(StateController* ctrl) {
    ctrl->set_rgbLED(255, 0, 0); // red
    if (!this->initialized) {this->initialize(ctrl);}
    this->advertise(ctrl);
    // Serial.println("advertising!");
}

void AdvertisingState :: exit(StateController* ctrl) {
    ctrl->bf.Advertising.stop();
}

void AdvertisingState :: update(StateController* ctrl) {
    if (ctrl->is_connected()) {
        ctrl->devStatus = DEVICE_OK;
        ctrl->go_to_state(ProgrammingState::getInstance());
    }
}

void AdvertisingState :: initialize(StateController* ctrl) {
    ctrl->bf.autoConnLed(false);
    ctrl->bf.configPrphBandwidth(BANDWIDTH_MAX);

    ctrl->bf.begin();
    ctrl->bf.setTxPower(0);    // Check bluefruit.h for supported values

    ctrl->bf.Periph.setConnectCallback(connect_callback);
    ctrl->bf.Periph.setDisconnectCallback(disconnect_callback);

    this->bledfu.begin();

    // Configure and Start Device Information Service
    this->bledis.setManufacturer("Adafruit Industries");
    this->bledis.setModel("IB-nRF52840-X");
    this->bledis.begin();

    // Configure and Start BLE Uart Service
    ctrl->bleuart.begin();
    ctrl->bleuart.setRxCallback(uart_rx_callback);

    // Start BLE Battery Service
    this->blebas.begin();
    this->blebas.write(100);

    // Advertising packet
    ctrl->bf.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    ctrl->bf.Advertising.addTxPower();

    // Include bleuart 128-bit uuid
    ctrl->bf.Advertising.addService(ctrl->bleuart);
    ctrl->bf.ScanResponse.addName();

    ctrl->bf.Advertising.restartOnDisconnect(0);
    ctrl->bf.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    ctrl->bf.Advertising.setFastTimeout(30);      // number of seconds in fast mode
    
    this->initialized = true;
}

void AdvertisingState :: advertise(StateController* ctrl) {
    ctrl->bf.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void connect_callback(uint16_t conn_handle) {
    // Get the reference to current connection
    StateController* ctrl = &StateController::getInstance();
    ctrl->connection = conn_handle;
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
    (void) conn_handle;
    (void) reason;

    StateController* ctrl = &StateController::getInstance();
    ctrl->devStatus = DEVICE_NO_CONNECT; 
    ctrl->go_to_state(AdvertisingState::getInstance());
}

void uart_rx_callback(uint16_t conn_handle) {
    StateController* ctrl = &StateController::getInstance();
    ctrl->devStatus = DEVICE_INTERRUPT;
    ctrl->go_to_state(InterruptState::getInstance());
}
