import time
from pyduino import PyDuino


def main():
    devices = PyDuino.FindAll()
    for d in devices:
        print(d.address, d.name)

    print()

    v = 1
    while True:
        if v == 1:
            v = 0
            print('OFF ', end='')
        else:
            v = 1
            print('ON ', end='')

        for d in devices:
            d.digitalWrite(13, v)

        time.sleep(1)



if __name__ == '__main__':
    main()