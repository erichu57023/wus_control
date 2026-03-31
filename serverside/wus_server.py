# This class provides a remote server controller to interface with a wireless ultrasound (WUS) device through Bluetooth 
# Low Energy (BLE) for experiment design.

import asyncio
import struct
from constants import *
from bleak import BleakScanner
from bleak_retry_connector import establish_connection, BleakClientWithServiceCache



class WUS_Server:
    __slots__ = ["address", "client", "device"]

    def __init__(self, address=None):
        self.address = address
        self.client = None
        self.device = None

    def __enter__(self):
        self.connect()
        return self
    
    def __exit__(self, exc_type, exc_value, ):
        self.disconnect()
        if exc_type is not None:
            print(f"An exception occurred within 'with' block: {exc_value}")
        return False

    def __del__(self):
        self.disconnect()

    def __read(self, command_uuid):
        return asyncio.run(self.client.read_gatt_char(command_uuid))
    
    def __write(self, command_uuid, value_bytes):
        asyncio.run(self.client.write_gatt_char(command_uuid, value_bytes, response=False))


    def connect(self, timeout=10.0):
        if self.client is None:
            if self.device is None:
                discovered_device = asyncio.run(BleakScanner.find_device_by_name(name=DEVICE_NAME, timeout=timeout))
                if discovered_device is None:
                    raise Exception(f"WUS_Server.connect: WUS device not found within {timeout} s.")
                else:
                    print(f"Discovered WUS device #{discovered_device.address}")
                    self.device = discovered_device
                    self.address = discovered_device.address

        self.client = asyncio.run(establish_connection(
            BleakClientWithServiceCache, self.device, name=DEVICE_NAME, timeout=timeout, pair=True))
        
        if self.client is None:
            raise Exception(f"WUS_Server.connect: WUS #{self.address} not found.")
        
        print(f"Connected to WUS #{self.address}")
        
        # After initial sync, switch to slow intervals to save battery
        asyncio.run(self.client.set_connection_params(
            min_interval=800,   # 1000ms
            max_interval=800,   # 1000ms
            latency=0,
            timeout=600,        # 6000ms
        ))

    def disconnect(self):
        self.stop()
        if self.client is not None and self.client.is_connected:
            asyncio.run(self.client.disconnect())
            print("Disconnected from WUS device.")

    def start(self):
        asyncio.run(self.client.write_gatt_char(ENABLE_UUID, b"\x01", response=False))

    def stop(self):
        asyncio.run(self.client.write_gatt_char(ENABLE_UUID, b"\x00", response=False))


    # Read-only setting getters
    @property
    def is_connected(self):
        return self.client.is_connected if self.client is not None else False
    
    @property
    def is_enabled(self):
        return bool(int.from_bytes(self.__read(ENABLE_UUID), byteorder="little"))

    @property
    def cs_tuss4470(self):
        return int.from_bytes(self.__read(TUSS4470_UUID), byteorder="little")
    
    @property
    def cs_ad9833(self):
        return int.from_bytes(self.__read(AD9833_UUID), byteorder="little")

    @property
    def cs_burst_ctrl(self):
        return int.from_bytes(self.__read(BURST_CTRL_UUID), byteorder="little")
    
    @property
    def regulated_mode(self):
        return bool(int.from_bytes(self.__read(REGULATED_MODE_UUID), byteorder="little"))

    @property
    def pre_driver_mode(self):
        return bool(int.from_bytes(self.__read(PRE_DRIVER_MODE_UUID), byteorder="little"))

    @property
    def hi_current_mode(self):
        return bool(int.from_bytes(self.__read(CURRENT_MODE_UUID), byteorder="little"))

    @property
    def io_mode(self):
        return int.from_bytes(self.__read(IO_MODE_UUID), byteorder="little")


    # Read-write setting getters
    @property
    def voltage_v(self):
        return int.from_bytes(self.__read(VOLTAGE_UUID), byteorder="little")

    @property
    def pulse_count(self):
        return int.from_bytes(self.__read(PULSE_CT_UUID), byteorder="little")

    @property
    def frequency_hz(self):
        return struct.unpack("<f", self.__read(FREQUENCY_UUID))[0]
    
    @property
    def timeout_ms(self):
        return int.from_bytes(self.__read(TIMEOUT_UUID), byteorder="little")
    
    @property
    def burst_period_us(self):
        return int.from_bytes(self.__read(BURST_PD_UUID), byteorder="little")

    @property
    def stim_period_us(self):
        return int.from_bytes(self.__read(STIM_PD_UUID), byteorder="little")

    @property
    def burst_dc(self):
        return struct.unpack("<f", self.__read(BURST_DC_UUID))[0]

    @property
    def stim_dc(self):
        return struct.unpack("<f", self.__read(STIM_DC_UUID))[0]
    
    @property
    def dc_sequence(self):
        seq = list(asyncio.run(self.client.read_gatt_char(DCSEQ_UUID)))
        seq = [round(x * 100 / 255, 2) for x in seq]  # Scale back from 0-255 to 0-100%
        return seq

    # Read-write setting setters
    @voltage_v.setter
    def voltage_v(self, value : int):
        assert 5 <= value <= 20, "Voltage (V) must be between 5 and 20."
        self.__write(VOLTAGE_UUID, int(value).to_bytes(1, byteorder="little", signed=False))

    @pulse_count.setter
    def pulse_count(self, value : int):
        assert 0 <= value <= 63, "Pulse count must be between 0 and 63."
        self.__write(PULSE_CT_UUID, int(value).to_bytes(1, byteorder="little", signed=False))

    @frequency_hz.setter
    def frequency_hz(self, value : float):
        assert value > 0, "Frequency (Hz) must be positive."
        self.__write(FREQUENCY_UUID, struct.pack("<f", value))

    @timeout_ms.setter
    def timeout_ms(self, value : int):
        assert 0 <= value <= 0xFFFFFFFF, "Timeout (ms) must fit in a uint32."
        self.__write(TIMEOUT_UUID, int(value).to_bytes(4, byteorder="little", signed=False))

    @burst_period_us.setter
    def burst_period_us(self, value : int):
        assert 0 <= value <= 0xFFFFFFFF, "Burst pulse duration (us) must fit in a uint32."
        self.__write(BURST_PD_UUID, int(value).to_bytes(4, byteorder="little", signed=False))

    @stim_period_us.setter
    def stim_period_us(self, value : int):
        assert 0 <= value <= 0xFFFFFFFF, "Stim pulse duration (us) must fit in a uint32."
        self.__write(STIM_PD_UUID, int(value).to_bytes(4, byteorder="little", signed=False))

    @burst_dc.setter
    def burst_dc(self, value : float):
        assert 0.0 < value <= 100.0, "Burst duty cycle (%) must be between 0 and 100."
        self.__write(BURST_DC_UUID, struct.pack("<f", value))

    @stim_dc.setter
    def stim_dc(self, value : float):
        assert 0.0 < value <= 100.0, "Stim duty cycle (%) must be between 0 and 100."
        self.__write(STIM_DC_UUID, struct.pack("<f", value))

    @dc_sequence.setter
    def dc_sequence(self, value : list):
        assert isinstance(value, list) and \
            all(isinstance(x, (float, int)) and 0.0 <= x <= 100.0 for x in value), \
            "DC sequence must be a list of floats between 0 and 100."
        value_uint8 = [round(x * 255 / 100) for x in value]  # Scale duty cycle from 0-100% to 0-255
        if value_uint8[-1] != 0:  # Ensure the sequence ends with a 0% duty cycle to prevent continuous stimulation
            value_uint8.append(0)
        self.__write(DCSEQ_UUID, bytes(value_uint8))
        print(bytes(value_uint8))

    def disable_dc_sequence(self):
        self.__write(DCSEQ_UUID, b"\x00")

    def get_all_settings(self):
        settings = {
            "is_enabled": self.is_enabled,
            "cs_tuss4470": self.cs_tuss4470,
            "cs_ad9833": self.cs_ad9833,
            "cs_burst_ctrl": self.cs_burst_ctrl,
            "regulated_mode": self.regulated_mode,
            "pre_driver_mode": self.pre_driver_mode,
            "hi_current_mode": self.hi_current_mode,
            "io_mode": self.io_mode,
            "voltage_v": self.voltage_v,
            "pulse_count": self.pulse_count,
            "frequency_hz": self.frequency_hz,
            "timeout_ms": self.timeout_ms,
            "burst_period_us": self.burst_period_us,
            "stim_period_us": self.stim_period_us,
            "burst_dc": self.burst_dc,
            "stim_dc": self.stim_dc,
            "dc_sequence": self.dc_sequence
        }
        return settings
