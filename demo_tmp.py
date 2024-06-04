# Import the PyDuino library
from pyduino import *

# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)

my_esp = devices['UNO_1']

# Loop forever
while True:

    # Turn the lamp off
    my_esp.digitalWrite(13, 1)
    print("Inverter turned on")

    # Wait 5 seconds
    time.sleep(1)

    # Turn the lamp on
    my_esp.digitalWrite(13, 0)
    print("Inverter turned off")

    # Wait 5 seconds
    time.sleep(1)