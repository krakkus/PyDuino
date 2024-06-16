# Import the PyDuino library
from pyduino import *

# Discover devices on local subnet and COM ports
devices = PyDuino.FindAll()

# Show us all discovered devices
for k, v in devices.items():
    print(k, v.address)

my_esp = devices['ESP32_1']

# Loop forever
while True:

    # Turn motor one way
    my_esp.digitalWrite(2, 0)
    my_esp.digitalWrite(4, 1)
    print("GPIO_2 OFF & GPIO_4 ON\n")

    # Wait 2 seconds
    time.sleep(2)

    # Turn motor other way
    my_esp.digitalWrite(2, 1)
    my_esp.digitalWrite(4, 0)

    print("GPIO_2 ON & GPIO_4 OFF\n")

    # Wait 2 seconds
    time.sleep(2)