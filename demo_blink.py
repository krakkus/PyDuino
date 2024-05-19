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
    pot = devices['ESP32_1'].digitalWrite(2, 0)
    print("Light turned on")

    # Wait 5 seconds
    time.sleep(5)

    # Turn the lamp on
    pot = devices['ESP32_1'].digitalWrite(2, 1)
    print("Light turned off")

    # Wait 5 seconds
    time.sleep(5)
