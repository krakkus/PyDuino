# PyDuino, C&C for your Arduino/ESP fleet

PyDuino is a tool that will make discovering and controlling multiple Arduino or ESP devices a walk in the park.

    
    from pyduino import *
    
    devices = PyDuino.FindAll()
    
    for k, v in devices.items():
        print(k, v.address)

This for example will return all available devices in your local subnet or connected by serial.

    ESP8266_1 192.168.1.12
    NANO_1 COM3

In my case two devices are discovered and now ready to be controlled. Let's have a look at the following code.

    from pyduino import *
    
    devices = PyDuino.FindAll()
    
    for k, v in devices.items():
        print(k, v.address)
    
    devices['ESP8266_1'].pinMode(0, INPUT)
    devices['NANO_1'].pinMode(9, OUTPUT)
    
    while True:
        pot = devices['ESP8266_1'].analogRead(0)
    
        led = int(pot * 255 / 1023)
    
        devices['NANO_1'].analogWrite(9, led)
    
        print(f'\rPot: {pot}, LED {led}   ', end='')
    
This will have the potentiometer on one device, control the brightness of an LED on another device.

![](https://krakkus.com/wp-content/uploads/2023/05/PXL_20230505_170058312.jpg)

## Installation

1. Download the .zip file from the GitHub [repository](https://github.com/krakkus/PyDuino)
2. Copy the PyDuino folder into your Python project 
3. From the sketches folder select the proper sketch, edit some details like devicename or Wi-Fi settings
4. Upload the sketch

# Supported functions

The PyDuino.FindAll() function returns a dictonary of PyDuino objects. You can send the following commands to them:

* pinMode(pin, mode)
* digitalRead(pin)
* digitalWrite(pin, value)
* analogRead(pin)
* analogWrite(pin, value)

# Some constants are available

    HIGH = 1
    LOW = 0
    
    INPUT_PULLDOWN = 4
    INPUT_PULLUP = 2
    OUTPUT = 1
    INPUT = 0
