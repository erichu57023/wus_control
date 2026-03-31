from wus_control.state_controller import StateController
from wus_control.advertising_state import AdvertisingState
from wus_control.programming_state import ProgrammingState
from wus_control.interrupt_state import InterruptState
from wus_control.bursting_state import BurstingState
from wus_control.idle_state import IdleState

settings_file = 'settings.txt'
print('Hello, world!')

device = StateController(settings_file)
device.add_state(AdvertisingState())
device.add_state(ProgrammingState())
device.add_state(InterruptState())
device.add_state(BurstingState())
device.add_state(IdleState())

device.go_to_state('advertising')
while True:
    device.update()
