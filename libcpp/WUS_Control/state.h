#ifndef STATE_H
#define STATE_H

enum stateName {NULL_STATE, ADVERTISING_STATE, BURSTING_STATE, IDLE_STATE, PROGRAMMING_STATE};
enum deviceStatus {DEVICE_OK, DEVICE_NO_CONNECT, DEVICE_INTERRUPT};

class State {
    public:
        virtual ~State(void) {};
        virtual stateName get_name(void);
        virtual void enter(void);
        virtual void exit(void);
        virtual void update(void);
};

#endif
