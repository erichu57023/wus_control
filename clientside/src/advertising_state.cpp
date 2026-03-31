#include "advertising_state.h"
#include "state_controller.h"

stateName AdvertisingState :: get_name(void) {
    return ADVERTISING_STATE;
}

void AdvertisingState :: enter(StateController* ctrl) {
    ctrl->set_rgbLED(255, 0, 0); // red
    if (!this->initialized) {this->initialize(ctrl);}
    this->advertise(ctrl);
}

void AdvertisingState :: exit(StateController* ctrl) {
    ctrl->bf.Advertising.stop();
}

void AdvertisingState :: update(StateController* ctrl) {
    if (ctrl->is_connected()) {
        ctrl->devStatus = DEVICE_OK;
        ctrl->go_to_state(IDLE_STATE);
    }
    Serial.println("Advertising!");
}

void AdvertisingState :: initialize(StateController* ctrl) {
    ctrl->bf.autoConnLed(false);
    ctrl->bf.configPrphBandwidth(BANDWIDTH_NORMAL);

    ctrl->bf.begin();
    ctrl->bf.setName(DEVICE_NAME);
    ctrl->bf.setTxPower(0);    // nrf52840 supported values: [-40,-20,-16,-12,-8,-4,0,+2,+3,+4,+5,+6,+7,+8] dBm

    ctrl->bf.Periph.setConnectCallback(connect_callback);
    ctrl->bf.Periph.setDisconnectCallback(disconnect_callback);

    // Configure and Start Device Information Service
    this->bledis.setManufacturer("Adafruit Industries");
    this->bledis.setModel("IB-nRF52840-X");
    this->bledis.begin();

    /////////// CONSTANT SETTINGS ///////////
    SettingManager* sets = ctrl->settings;

    // Configure and start a generic service for storing constant settings
    this->constSetServ = BLEService(CONST_SETTING_SRV_UUID);
    this->constSetServ.begin();

    // Configure and start individual characteristics for each setting, read-only
    const BLECharacteristic constSets[] = {this->csTUSS4470Set, this->csAD9833Set, this->csBurstCTRLSet, this->regModeSet,
                                           this->preDriverModeSet, this->currentModeSet, this->ioModeSet}; 
    const uint16_t constSetUUIDs[] = {TUSS4470_UUID, AD9833_UUID, BURST_CTRL_UUID, REGULATED_MODE_UUID,
                                      PRE_DRIVER_MODE_UUID, CURRENT_MODE_UUID, IO_MODE_UUID};
    const uint8_t  constSetVals[]  = {sets->cs_tuss4470, sets->cs_ad9833, sets->cs_burst_control, sets->regulated_mode,
                                      sets->pre_driver_mode, sets->current_mode, sets->io_mode};
    String constSetDescs[] = {"TUSS4470 CS Pin", "AD9833 CS Pin", "Burst Control Pin", "Regulated Mode",
                              "Pre-Driver Mode", "Hi-Current Mode", "Burst I/O Mode"};
    for (unsigned int i = 0; i < sizeof(constSetUUIDs) / 2; i++) {
        BLECharacteristic constSet = constSets[i];
        constSet = BLECharacteristic(constSetUUIDs[i], CHR_PROPS_READ, 1, true); // Assumes all constant settings are 1 byte
        constSet.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
        constSet.setUserDescriptor(constSetDescs[i].c_str());
        constSet.begin();
        constSet.write8(constSetVals[i]);
    }

    /////////// CONSTANT SETTINGS ///////////
    // Configure and start a generic service for storing constant settings
    this->mutSetServ = BLEService(MUT_SETTING_SRV_UUID);
    this->mutSetServ.begin();

    // Configure and start a generic service for storing small mutable (runtime) settings
    BLECharacteristic* mutSets[] = {&ctrl->reportEnabled, &this->voltageSet, &this->pulseCountSet, &this->frequencySet, &this->timeoutSet,
                                    &this->burstPDSet, &this->stimPDSet, &this->burstDCSet, &this->stimDCSet}; 
    const uint16_t mutSetUUIDs[] = {ENABLE_UUID, VOLTAGE_UUID, PULSE_CT_UUID, FREQUENCY_UUID, TIMEOUT_UUID,
                                    BURST_PD_UUID, STIM_PD_UUID, BURST_DC_UUID, STIM_DC_UUID};
    uint32_t freqInt, burstDCInt, stimDCInt;  // Floats need to be bit-cast to uint32
    memcpy(&freqInt,    (const float*) &sets->frequency,  4);
    memcpy(&burstDCInt, (const float*) &sets->burst_dc,   4);
    memcpy(&stimDCInt,  (const float*) &sets->stim_dc,    4);
    const uint32_t mutSetVals[]  = {0, sets->voltage, sets->pulse_count, freqInt, sets->timeout,
                                    sets->burst_pd, sets->stim_pd, burstDCInt, stimDCInt};
    const uint8_t mutSetLens[]  = {1, 1, 1, 4, 4, 4, 4, 4, 4};
    char mutSetDescs[][18] = {"Burst Enabled", "Voltage", "Pulse Count", "Frequency (Hz)", "Timeout (ms)",
                              "Burst Period (us)", "Stim Period (us)", "Burst DC (%)", "Stim DC (%)"};
    for (unsigned int i = 0; i < sizeof(mutSetUUIDs) / 2; i++) {
        BLECharacteristic* mutSet = mutSets[i];
        *mutSet = BLECharacteristic(mutSetUUIDs[i], RD_WTNR_NOTIFY, mutSetLens[i], true);
        mutSet->setPermission(SECMODE_OPEN, SECMODE_OPEN);
        mutSet->setUserDescriptor(mutSetDescs[i]);
        mutSet->setWriteCallback(setting_rx_callback);
        mutSet->begin();
        mutSet->write32(mutSetVals[i]);
    }

    // Configure service for long variable-length DC sequence characteristic
    BLECharacteristic* dcSeqChar = &this->dcSeqSet;
    *dcSeqChar = BLECharacteristic(DCSEQ_UUID, RD_WTNR_NOTIFY, sets->dc_seq_len, false);
    dcSeqChar->setPermission(SECMODE_OPEN, SECMODE_OPEN);
    dcSeqChar->setUserDescriptor("Custom DC Sequence");
    dcSeqChar->setWriteCallback(setting_rx_callback);
    dcSeqChar->begin();
    dcSeqChar->write((void*) sets->dc_seq_vals, sets->dc_seq_len);

    // Advertising packet
    ctrl->bf.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    ctrl->bf.Advertising.addTxPower();

    // Include UUIDs
    ctrl->bf.Advertising.addService(this->constSetServ, this->mutSetServ);
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
    ctrl->go_to_state(ADVERTISING_STATE);
}

void setting_rx_callback(uint16_t conn_handle, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
    StateController* ctrl = &StateController::getInstance();

    // Identify source UUID
    uint16_t rx_uuid = 0;
    chr->uuid.get(&rx_uuid);
    uint32_t rx_data = 0;

    // Aggregate incoming bytes into a uint32 for up to 4-byte settings
    if (len <= 4) {
        memcpy(&rx_data, data, len);
    }

    if (rx_uuid == ENABLE_UUID) {
        if (rx_data) {
            ctrl->go_to_state(BURSTING_STATE);
        } else {
            ctrl->go_to_state(IDLE_STATE);
        }
        return;
    
    } else {
        ctrl->settings->repgmKey = static_cast<mutableSetting>(rx_uuid);
        ctrl->devStatus = DEVICE_INTERRUPT;
        // Pass pointer to DCSEQ array directly since repgmVal can only store a uint32
        if (SettingManager::repgmKey == DCSEQ_CHG) {
            SettingManager::dc_seq_len = len;
            SettingManager::repgmVal = reinterpret_cast<uint32_t>(&data);
        } else {
            SettingManager::repgmVal = rx_data;
        }
        ctrl->go_to_state(PROGRAMMING_STATE);
    }    
}