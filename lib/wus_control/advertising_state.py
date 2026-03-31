import board
from wus_control.state import State
from digitalio import DigitalInOut, Direction
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

class AdvertisingState(State):

    def __init__(self):
        self.red_led = DigitalInOut(board.LED)
        self.red_led.direction = Direction.OUTPUT

        self.ble = BLERadio()
        self.uart = UARTService()
        self.advertisement = ProvideServicesAdvertisement(self.uart)

    @property
    def name(self):
        return 'advertising'

    def enter(self, controller):
        State.enter(self, controller)
        self.red_led.value = True
        controller.rgb_led[0] = (0, 0, 0)
        print("Advertising...")
        controller.ble = self.ble;
        controller.uart = self.uart;
        self.ble.start_advertising(self.advertisement)

    def exit(self, controller):
        State.exit(self, controller)
        self.red_led.value = False
        self.ble.stop_advertising()
        print("\tConnected!")

    def update(self, controller):
        if not State.update(self, controller) == 'NO_CONNECT':
            controller.go_to_state('programming')
