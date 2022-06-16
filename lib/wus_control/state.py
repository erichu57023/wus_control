class State(object):

    def __init__(self):
        pass

    @property
    def name(self):
        return ''

    def enter(self, controller):
        pass

    def exit(self, controller):
        pass

    def update(self, controller):
        if not controller.ble.connected:
            return 'NO_CONNECT'
        if controller.uart.in_waiting:
            return 'INTERRUPT'
        return 'OK'
