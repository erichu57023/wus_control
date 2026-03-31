# Author: Eric Hu
# Date: 7/6/22

# This code provides a controller to interface with a wireless ultrasound (WUS) device for experiment design,
# running the code at github.com/erichu57023/wus_control. For an example of how to interface, see the main block.

# IMPORTANT: This code is only confirmed to work on Python 3.8; newer versions may result in cryptic errors.
# The following packages have been installed:
#  pip install --upgrade adafruit-blinka-bleio adafruit-circuitpython-ble
# If pythonnet installation errors, run
#  pip install --pre pythonnet

from sys import version, version_info
from warnings import warn
if version_info[0] < 3:
    print(version)
    raise Exception("Must use Python 3")
elif version_info[1] != 8:
    warn('This code is only confirmed to work on Python 3.8; other versions may result in cryptic errors.')

from time import monotonic_ns
from time import monotonic as time_now

try:
    from adafruit_ble import BLERadio
    from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
    from adafruit_ble.services.nordic import UARTService
except ImportError:
    print('Couldn\'t find the required packages!')
    print('Run the following from command-line:')
    print('pip install --upgrade adafruit-blinka-bleio adafruit-circuitpython-ble')
    exit(1)


class WUS_Controller:
    valid_commands = ['on', 'off', 'cs_ad9833', 'cs_tuss4470', \
        'cs_burst_control', 'regulated_mode', 'pre_driver_mode', \
        'voltage', 'current_mode', 'io_mode', \
        'frequency', 'timeout', 'burst_period', \
        'pulse_count', 'duty_cycle', 'setting']
    
    def __init__(self):
        self._ble = BLERadio()
        self._uart_connection = None
        self._uart_service = None

    def connect(self):
        if not self._uart_connection:
            print("Trying to connect...")
            for adv in self._ble.start_scan(ProvideServicesAdvertisement):
                if UARTService in adv.services:
                    self._uart_connection = self._ble.connect(adv)
                    print("Connected")
                    break
            self._ble.stop_scan()

        if self._uart_connection and self._uart_connection.connected:
            self._uart_service = self._uart_connection[UARTService]

    def check_connection(self):
        return (self._uart_connection and self._uart_connection.connected)

    def shutdown(self):
        print('Disconnecting...')
        self._uart_service.reset_input_buffer()
        self._uart_connection.disconnect()
        print('Done!')

    def set(self, command, value):
        if command not in self.valid_commands:
            print('WARNING: \'' + command + '\' is not a valid command.')
        else:
            if command in ['on', 'off', 'settings']:
                message = command + '\n'
            else:
                message = command + '=' + str(value) + '\n'
            self._uart_service.write(message.encode('utf-8'))

    def check_setting(self, setting):
        self.set('setting', setting)

    def hold_on(self, timeout_secs, refresh_interval=0.5):
        timeout_ns = timeout_secs * 1e9
        try:
            start_time = monotonic_ns()
            while monotonic_ns() - start_time < timeout_ns:
                self._burst_on()
                self.wait(refresh_interval)
            self._burst_off()
        except KeyboardInterrupt:
            raise

    def wait(self, timeout_secs):
        timeout_ns = timeout_secs * 1e9
        try:
            start_time = monotonic_ns()
            while monotonic_ns() - start_time < timeout_ns:
                self._read_if_available()
        except KeyboardInterrupt:
            raise

    def _read_if_available(self):
        try:
            numbytes = self._uart_service.in_waiting
            while numbytes:
                print(self._uart_service.read(numbytes).decode("utf-8"), end='')
                numbytes = self._uart_service.in_waiting
        except KeyboardInterrupt:
            raise

    def _burst_on(self):
        self.set('on', None)

    def _burst_off(self):
        self.set('off', None)




# Example connection
if __name__ == '__main__':
    try:
        exp = WUS_Controller()
        exp.connect()
        exp.set('pulse_count', 0)
        exp.set('frequency', 440000)
        exp.set('timeout', 30)
        exp.check_setting('frequency')
        exp.check_setting('timeout')

        while True:
            if not exp.check_connection():
                break;
            exp.hold_on(4)
            exp.wait(1)

    except KeyboardInterrupt:
        print('CTRL-C detected')

    exp.shutdown()
