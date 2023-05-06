from pyduino import *


devices = PyDuino.FindAll()

for k, v in devices.items():
    print(k, v.address)

devices['ESP8266_1'].pinMode(0, INPUT)

while True:
    pot = devices['ESP8266_1'].analogRead(0)

    servo = int(pot * 180 / 1023)

    devices['NANO_1'].servoWrite(3, servo)

    print(f'\rPot: {pot}, Servo {servo}   ', end='')
