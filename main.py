import time
import random
from pyduino import *


def main():
    devices = PyDuino.FindAll()
    for d in devices:
        print(d)
        d.pinMode(14, INPUT)

    v = 1
    while True:
        if v == 1:
            v = 0
        else:
            v = 1

        for d in devices:
            i = d.digitalRead(14)
            print(i)

        time.sleep(1)



if __name__ == '__main__':
    main()