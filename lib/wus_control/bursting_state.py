import time
import board
from wus_control.state import State

class BurstingState(State):
    
    def __init__(self):
        self._ctrl_pin = None
        self._counter = None
        self._duty_cycle = 50

    @property
    def name(self):
        return 'bursting'

    def enter(self, controller):
        controller.rgb_led[0] = (0, 0, 255)
        State.enter(self, controller)
        self._set_parameters(controller)
        self._start_burst = time.monotonic()
        self._start_pulse = self._start_burst
        print('Bursting!')

    def exit(self, controller):
        State.exit(self, controller)
        self._off()

    def update(self, controller):
        status = State.update(self, controller)
        if status == 'NO_CONNECT':
            controller.go_to_state('advertising')
        elif status == 'INTERRUPT':
            controller.go_to_state('interrupt')
        elif self._burst_elapsed() > self._timeout:
            print('\tTimeout')
            controller.go_to_state('idle')
        else:
            if self._pulse_elapsed() > self._period:
                self._start_pulse = time.monotonic()
                self._on()
            elif self._pulse_elapsed() > self._on_time:
                self._off()

    def _set_parameters(self, controller):
        self._ctrl_pin = controller.settings['cs_burst_control']
        self._off()
        self._timeout = controller.settings['timeout']
        self._period = controller.settings['burst_period'] / 1000
        if not controller.settings['pulse_count']:
            self._duty_cycle = controller.settings['duty_cycle']
        self._on_time = self._period * self._duty_cycle

    def _on(self):
        self._ctrl_pin.value = False

    def _off(self):
        self._ctrl_pin.value = True

    def _burst_elapsed(self):
        return time.monotonic() - self._start_burst

    def _pulse_elapsed(self):
        return time.monotonic() - self._start_pulse
