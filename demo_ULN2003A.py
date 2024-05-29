# Import the PyDuino library
from pyduino import *

# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)

esp = devices['ESP32_1']

esp.stepperWrite_2(15, 2, 4, 18, 5, 1000, 3)
