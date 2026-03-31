# TUSS4470.py - Library for controlling ultrasound burst excitation with TUSS4470. Listening is not supported.
# Author: Eric Hu, 6/14/2022

from adafruit_bus_device.spi_device import SPIDevice

RESET_DICT = {0x10:0x00, 0x11:0x00, 0x12:0x00, 0x13:0x00, 0x14:0x00, 0x16:0x20, 0x17:0x07, 0x18:0x14, 0x1A:0x00, 0x1B:0x00}
RW = {'READ': 0x8000, 'WRITE': 0x0000}
CURRENT = {'LOW': 0, 'HIGH': 1}
HIZ = {'OFF': 0, 'ON': 1}
IO = {'MODE0': 0, 'MODE1': 1, 'MODE2': 2, 'MODE3': 3}
PRE_DRIVER = {'OFF': 0, 'ON': 1}

# Create a TUSS4470 object
class TUSS4470(object):

    def __init__(self, cs_pin):
        # Chip select pin to enable SPI (active low)
        self._cs = cs_pin
        self._device = None

        self._voltage = None
        self._pulse_count = None
        self._hi_current_mode = CURRENT['LOW']
        self._hi_z_mode = HIZ['ON']
        self._io_mode = IO['MODE0']
        self._pre_driver_mode = PRE_DRIVER['OFF']

# ----------------------------- General functions ----------------------------- #

    def begin(self, spi_port):
        self._device = SPIDevice(spi=spi_port, chip_select=self._cs, baudrate=500000, polarity=0, phase=1)

    def reset(self):
        for key in RESET_DICT:
            self.control_register(RW['WRITE'], key, RESET_DICT[key]);
        self._voltage = 0;
        self._pulse_count = 0
        self._hi_current_mode = CURRENT['LOW']
        self._hi_z_mode = HIZ['ON']
        self._io_mode = IO['MODE0']
        self._pre_driver_mode = PRE_DRIVER['OFF']

    def disableRegulation(self):
        self._hi_z_mode = HIZ['ON']
        self._update_VRDV_register()

    def enableRegulation(self):
        self._hi_z_mode = HIZ['OFF']
        self._update_VRDV_register()

    def disablePreDriver(self):
        self._pre_driver_mode = PRE_DRIVER['OFF'];
        self._update_BP_register()

    def enablePreDriver(self):
        self._pre_driver_mode = PRE_DRIVER['ON'];
        self._update_BP_register()

    def set(self, voltage, current, io_mode, pulse_count):
        self.set_voltage(voltage);
        self.set_current(current);
        self.set_io_mode(io_mode);
        self.set_pulse_count(pulse_count);

    def set_voltage(self, voltage):
        voltage = max(voltage - 5, 0)   # Voltage must be at least 5, if not it will default to 5
        voltage = voltage % 16          # Voltage is represented by a 4-bit number, so max is 21
        self._voltage = int(voltage)
        self._update_VRDV_register()

    def set_current(self, current_type):
        self._hi_current_mode = CURRENT[current_type]
        self._update_VRDV_register()

    def set_io_mode(self, io_mode):
        self._io_mode = IO[io_mode]
        self._update_DC_Register()

    def set_pulse_count(self, pulse_count):
        pulse_count = pulse_count % 64  # Pulse count is a 6-bit number, so max is 63
        self._pulseCount = int(pulse_count)
        self._update_BP_register()

# ----------------------------- Private functions ----------------------------- #

    def _update_VRDV_register(self):
        data = (self._hi_z_mode << 5) + (self._hi_current_mode << 4) + self._voltage
        self._control_register(RW['WRITE'], 0x16, data)

    def _update_BP_register(self):
        data = self._pulseCount
        if self._pre_driver_mode:
            data += 0x40
        self._control_register(RW['WRITE'], 0x1A, data)

    def _update_DC_Register(self):
        data = self._io_mode
        self._control_register(RW['WRITE'], 0x14, data)

    def _control_register(self, rw, address, data):
        address = address & 0x3F;
        comm = rw + (address << 9) + data
        if not self._find_parity16(comm):
            comm = comm + 0x0100
        self._transfer_command(comm);

    def _find_parity16(self, num):
        num = num ^ (num >> 8)
        num = num ^ (num >> 4)
        num = num ^ (num >> 2)
        num = num ^ (num >> 1)
        return num & 1

    def _transfer_command(self, data16):
        data_bytes = bytearray([data16 >> 8, data16 & 0x00FF])
        rcv_bytes = bytearray(2)
        with self._device as spi:
            spi.write_readinto(data_bytes, rcv_bytes)
        rcv16 = int.from_bytes(rcv_bytes, 'big')
        # print(bin(data16))
        # print(bin(rcv16))
        # print()
