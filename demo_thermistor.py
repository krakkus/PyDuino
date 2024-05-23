# Import the PyDuino library
from pyduino import *

# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)

# esp = devices['ESP32_1']
#
# esp.digitalWrite(34, HIGH)
# esp.digitalWrite(32, LOW)
#
# a = esp.analogRead(35)
#
# print(a)