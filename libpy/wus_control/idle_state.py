from wus_control.state import State

class IdleState(State):
    
    def __init__(self):
        pass

    @property
    def name(self):
        return 'idle'

    def enter(self, controller):
        State.enter(self, controller)
        controller.rgb_led[0] = (0, 255, 0)

    def exit(self, controller):
        State.exit(self, controller)

    def update(self, controller):
        status = State.update(self, controller)
        if status == 'NO_CONNECT':
            controller.go_to_state('advertising')
        elif status == 'INTERRUPT':
            controller.go_to_state('interrupt')
