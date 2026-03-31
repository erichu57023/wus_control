import board
import adafruit_dotstar as dotstar
from collections import OrderedDict
from digitalio import DigitalInOut, Direction

identity = lambda x: x
valid_settings = {'cs_ad9833':int, 'cs_tuss4470':int, \
        'cs_burst_control':int, 'regulated_mode':bool, \
        'pre_driver_mode':identity, 'voltage':int, \
        'current_mode':identity, 'io_mode':identity, \
        'frequency':float, 'timeout':float, \
        'burst_period':float, 'pulse_count':int, 'duty_cycle':float}

class StateController(object):    

    def __init__(self, settings_file):
        self.state = None
        self.states = {}
        self.ble = None
        self.uart = None
        self.settings = OrderedDict()
        self._load_settings(settings_file)
        self.previous_state = None
        self.reprogram_setting = None
        self.rgb_led = dotstar.DotStar(board.APA102_SCK, board.APA102_MOSI, 1, brightness=0.1)

    def add_state(self, state):
        self.states[state.name] = state

    def go_to_state(self, state_name):
        if self.state:
            self.previous_state = self.state.name
            self.state.exit(self)
        self.state = self.states[state_name]
        self.state.enter(self)

    def update(self):
        if self.state:
            self.state.update(self)

    def print(self, message):
        print(message)
        format_message = message.strip() + '\n'
        self.uart.write(format_message.encode('utf-8'))

    def _load_settings(self, settings_file):
        with open(settings_file, encoding='utf-8') as f:
            for line in f:
                if not (line.strip() or line[0].isalpha()):
                    continue
                setting = line.split()
                setting[0] = setting[0].lower()
                if setting[0] in valid_settings:
                    if setting[0].startswith('cs_'):    # Handle pin settings differently by initializing the pins first
                        setting[1] = eval('board.D' + str(setting[1]))
                        setting[1] = DigitalInOut(setting[1])
                        setting[1].direction = Direction.OUTPUT
                        setting[1].value = True
                    else:                               # Parse setting values based on datatype
                        setting[1] = valid_settings[setting[0]](setting[1])
                    self.settings[setting[0]] = setting[1]
