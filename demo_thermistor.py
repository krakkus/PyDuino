# Import the PyDuino library
from pyduino import *

# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)

esp = devices['ESP32_1']

esp.digitalWrite(35, 1)
esp.digitalWrite(33, 1)

a = esp.analogRead(32)
print(a)

