#include "advertising_state.h"

AdvertisingState :: AdvertisingState(StateController& controller) {
    this->ctrl = controller;
}

stateName AdvertisingState :: get_name(void) {
    return ADVERTISING_STATE;
}

void AdvertisingState :: enter(void) {
    Serial.println("entering adv");
    this->ctrl.set_rgbLED(255, 0, 0); // red
    setup_adv();
    start_adv();
}

void AdvertisingState :: exit(void) {
    Serial.println("exiting adv");
    // Bluefruit.Advertising.stop();
}

void AdvertisingState :: update(void) {
    if (Bluefruit.connected()) {
        this->ctrl.devStatus = DEVICE_OK;
        this->ctrl.go_to_state(PROGRAMMING_STATE);
    }
}

void AdvertisingState :: setup_adv(void) {
    Bluefruit.autoConnLed(false);
    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

    Bluefruit.begin(1, 0);
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

    // Configure and Start Device Information Service
    bledis.setManufacturer("Adafruit Industries");
    bledis.setModel("ItsyBitsy nRF52840 Express");
    bledis.begin();

    // Configure and Start BLE Uart Service
    ctrl.bleuart.begin();
}

void AdvertisingState :: start_adv(void) {
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();

    // Include bleuart 128-bit uuid
    Bluefruit.Advertising.addService(ctrl.bleuart);

    Bluefruit.Advertising.restartOnDisconnect(false);
    Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
    Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}
