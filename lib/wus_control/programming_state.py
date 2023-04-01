import board, busio, pwmio
from wus_control.state import State
from wus_control.hardware.AD9833 import AD9833
from wus_control.hardware.TUSS4470 import TUSS4470

class ProgrammingState(State):

    def __init__(self):
        # self._wave_gen = None
        self._wave_pwm = None
        self._burst_gen = None
        self._comm_port = None

    @property
    def name(self):
        return 'programming'

    def enter(self, controller):
        controller.rgb_led[0] = (255, 0, 0)
        State.enter(self, controller)
        if not self._burst_gen:
        # if not (self._wave_gen and self._burst_gen):    # When programming after advertising at startup
            self._startup_sequence(controller)
        elif controller.reprogram_setting:              # When programming single settings during runtime
            self._change_setting(controller)
        else:                                           # When entering programming state after readvertisement, no need to change settings
            pass

    def exit(self, controller):
        State.exit(self, controller)
        controller.reprogram_setting = None

    def update(self, controller):
        status = State.update(self, controller)
        if status == 'NO_CONNECT':
            controller.go_to_state('advertising')
        else:
            controller.go_to_state('idle')

    # Programs all settings
    def _startup_sequence(self, controller):
        controller.print("Startup: programming all settings...")
        st = controller.settings
        spi_port = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
        self._wave_gen = AD9833(st['cs_ad9833'])
        self._burst_gen = TUSS4470(st['cs_tuss4470'])

        # Program wave generator
        self._wave_pwm = pwmio.PWMOut(pin=st['pwm_wave_gen'], duty_cycle=2**15, frequency=int(2e6), variable_frequency=False)
        self._wave_gen.begin(spi_port)
        self._wave_gen.set_freq(st['frequency'])
        print(st['frequency'])
        self._wave_gen.set_type(1)
        self._wave_gen.send()

        # Program burst generator
        self._burst_gen.begin(spi_port)
        self._burst_gen.set(st['voltage'], st['current_mode'], st['io_mode'], st['pulse_count'])
        if st['regulated_mode']: self._burst_gen.enableRegulation()
        if st['pre_driver_mode']: self._burst_gen.enablePreDriver()
        controller.print('\tDone!')

    # If one setting gets changed during runtime, no need to reprogram all the settings
    def _change_setting(self, controller):
        setting = controller.reprogram_setting
        controller.print('Reprogramming ' + str(setting[0]) + ' to ' + str(setting[1]))
        controller.settings[setting[0]] = setting[1]
       
        if setting[0] =='frequency':
            self._wave_gen.set_freq(setting[1])
            self._wave_gen.send()
            # self._wave_pwm.frequency = setting[1]
        elif setting[0] == 'voltage':
            self._burst_gen.set_voltage(setting[1])
        elif setting[0] == 'pulse_count':
            self._burst_gen.set_pulse_count(setting[1])
        controller.print('\tDone!')
