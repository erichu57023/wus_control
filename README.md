# wus_control
Code for a wireless ultrasound transducer circuit

This code is designed for use with the Adafruit ItsyBitsy nRF52840 Express.
To get started, follow the instructions for loading the chip with CircuitPython ([instructions](https://learn.adafruit.com/adafruit-itsybitsy-nrf52840-express/circuitpython)), and then clone this repository into the board's flash memory.

This package uses a state machine representation to control the device, shifting between Advertising, Programming, Interrupt, Bursting, and Idle states. The main function is found in <code.py>. At startup, the device loads default settings from <SETTINGS.TXT>. Bursting activity is limited by a timeout, and must be continually maintained with on-signals from the host.

Also included is <serverside/wus_controller.py>, a module which provides an interface for the host device. It is only confirmed to work on Python 3.8; newer versions may result in unexpected errors.
