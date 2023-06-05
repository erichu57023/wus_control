# Author: Kipling Crossing
# Modified by: Eric Hu, 6/14/2022

from adafruit_bus_device.spi_device import SPIDevice

class AD9833(object):

    # Clock Frequency
    ClockFreq = 2000000
    freq = 10000
    shape_word = 0x2000

    def __init__(self, cs_pin):
        self._cs = cs_pin
        self._device = None

    def begin(self, spi_port):
        self._device = SPIDevice(spi=spi_port, chip_select=self._cs, baudrate=2500000, polarity=1, phase=0)

    def _send(self, data16):
        data_bytes = bytearray([data16 >> 8, data16 & 0x00FF])
        with self._device as spi:
            spi.write(data_bytes)

    def set_freq(self, freq):
        self._freq = freq

    def set_type(self, inter):
        if inter == 1:
            self.shape_word = 0x2028
        elif inter == 2:
            self.shape_word = 0x2002
        else:
            self.shape_word = 0x2000

    @property
    def shape_type(self):
        if self.shape_word == 0x2028:
            return "Square"
        elif self.shape_word == 0x2002:
            return "Triangle"
        else:
            return "Sine"

    def send(self):
        # Calculate frequency word to send
        word = round((self._freq * (2**28)) / self.ClockFreq)

        # Split frequency word onto its seperate bytes
        MSB = (word & 0xFFFC000) >> 14 
        LSB = (word & 0x3FFF)

        # Set control bits DB15 = 0 and DB14 = 1; for frequency register 0
        MSB |= 0x4000
        LSB |= 0x4000

        self._send(0x2100)
        # Set the frequency
        self._send(LSB)  # lower 14 bits
        self._send(MSB)  # Upper 14 bits
        # Set the shape
        # square: 0x2020, sin: 0x2000, triangle: 0x2002
        self._send(self.shape_word)
