from wus_control.state import State

mutable_settings = {'voltage':int, 'frequency':float, \
        'timeout':float, 'burst_period':float, \
        'pulse_count':int, 'duty_cycle':float}

class InterruptState(State):

    def __init__(self):
        pass

    @property
    def name(self):
        return 'interrupt'

    def enter(self, controller):
        State.enter(self, controller)

    def exit(self, controller):
        State.exit(self, controller)

    def update(self, controller):
        status = State.update(self, controller)
        if status == 'OK':
            controller.go_to_state('idle')
        if status == 'NO_CONNECT':
            controller.go_to_state('advertising')
        else:
            rcv = controller.uart.readline().decode('utf-8').split('=')
            rcv = [el.strip() for el in rcv]
            self._parse_command(controller, rcv)

    def _parse_command(self, controller, command):
        command[0] = command[0].lower()
        if command[0] == 'on':
            controller.go_to_state('bursting')
        elif command[0] == 'off':
            controller.go_to_state('idle')
        elif command[0] == 'settings':
            print('Settings: ' + str(controller.settings))
            controller.go_to_state('idle')
        else:
            if command[0] not in mutable_settings:
                raise KeyError('The requested setting does not exist.')
            command[1] = mutable_settings[command[0]](command[1])
            controller.reprogram_setting = command
            controller.go_to_state('programming')
