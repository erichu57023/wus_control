#ifndef ADVERTISING_STATE_H
#define ADVERTISING_STATE_H

#define RD_WTNR_NOTIFY 0b00010110

// Randomly generated UUIDs
#define CONST_SETTING_SRV_UUID  0xE8A5
#define MUT_SETTING_SRV_UUID    0x56F8
#define ENABLE_UUID             0xE65F

#include "state.h"
#include <bluefruit.h>

// Forward declarations
class StateController;
enum stateName : uint8_t;

class AdvertisingState: public State {
    public:
        // Singleton constructor methods
        static AdvertisingState& getInstance() {
            static AdvertisingState singleton;
            return singleton;
        }
        AdvertisingState(AdvertisingState const& other) = delete;
        AdvertisingState& operator = (const AdvertisingState& other) = delete;
        
        // Methods
        stateName get_name(void);
        void enter(StateController* ctrl);
        void exit(StateController* ctrl);
        void update(StateController* ctrl);

    private:
        // Constructor
        AdvertisingState(void) {};

        // Instance variables
        bool initialized = false;
        BLEDis bledis;
        BLEService constSetServ;
        BLECharacteristic csTUSS4470Set, csAD9833Set, csBurstCTRLSet, regModeSet,
                          preDriverModeSet, currentModeSet, ioModeSet;
        
        BLEService mutSetServ;
        BLECharacteristic voltageSet, pulseCountSet, frequencySet, timeoutSet, 
                          burstPDSet, stimPDSet, burstDCSet, stimDCSet, dcSeqSet;

        // Methods
        void initialize(StateController* ctrl);
        void advertise(StateController* ctrl);

};

void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);
void setting_rx_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len);

#endif
