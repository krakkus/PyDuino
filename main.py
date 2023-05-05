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
