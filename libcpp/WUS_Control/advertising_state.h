#ifndef ADVERTISING_STATE_H
#define ADVERTISING_STATE_H

#include "state_controller.h"
#include "state.h"
#include "programming_state.h"
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

class StateController;
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
        BLEDfu bledfu;
        BLEDis bledis;
        BLEBas blebas;

        // Methods
        void initialize(StateController* ctrl);
        void advertise(StateController* ctrl);
};

void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason); 
void uart_rx_callback(uint16_t conn_handle);

#endif
