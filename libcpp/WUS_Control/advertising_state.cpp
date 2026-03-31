#include "advertising_state.h"

AdvertisingState :: AdvertisingState(StateController* controller) 
    : ctrl(controller) {
        pinMode(RED_LED, OUTPUT);
        digitalWrite(RED_LED, LOW);
    }

void AdvertisingState :: enter() {
    State::enter();
    ctrl->set_rgbLED(0, 0, 0);

    setup_adv();
    start_adv();

    digitalWrite(RED_LED, HIGH);
}

void AdvertisingState :: exit() {
    State::exit();
    digitalWrite(RED_LED, LOW);
}

void AdvertisingState :: update() {
    State::update();
    if (ctrl->devStatus != DEVICE_NO_CONNECT) {
        ctrl->go_to_state(PROGRAMMING_STATE);
        ctrl->devStatus = DEVICE_OK;
    } else {
        ctrl->devStatus = DEVICE_NO_CONNECT;
    }
}

void AdvertisingState :: setup_adv() {
    Bluefruit.autoConnLed(true);
    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

    Bluefruit.begin();
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
    // Bluefruit.Periph.setConnectCallback(connect_callback);
    // Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

    // Configure and Start Device Information Service
    bledis.setManufacturer("Adafruit Industries");
    bledis.setModel("Bluefruit Feather52");
    bledis.begin();

    // Configure and Start BLE Uart Service
    bleuart.begin();

    // Start BLE Battery Service
    blebas.begin();
    blebas.write(100);
}

void AdvertisingState :: start_adv() {
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();

    // Include bleuart 128-bit uuid
    Bluefruit.Advertising.addService(bleuart);

    Bluefruit.Advertising.restartOnDisconnect(false);
    Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
    Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}
