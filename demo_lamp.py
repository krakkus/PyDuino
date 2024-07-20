# Import the PyDuino library
from pyduino import *

# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)

my_esp = devices['ESP32_HATCH']

# Loop forever
while True:

    # Turn the lamp off
    my_esp.digitalWrite(2, 1)
    print("Light turned on")

    # Wait 5 seconds
    time.sleep(5)

    # Turn the lamp on
    my_esp.digitalWrite(2, 0)
    print("Light turned off")

    # Wait 5 seconds
    time.sleep(5)