# Import the PyDuino library
from pyduino import *

# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)


# Loop forever
while True:

    # Turn the lamp off
    pot = devices['ESP32_1'].digitalWrite(1, 0)

    # Wait 5 seconds
    time.sleep(5)

    # Turn the lamp on
    pot = devices['ESP32_1'].digitalWrite(1, 1)

    # Wait 5 seconds
    time.sleep(5)
