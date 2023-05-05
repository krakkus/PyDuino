import time
import socket
import serial
import serial.tools.list_ports
import threading
import queue

TIMEOUT = 1

#LED_BUILTIN = 13

HIGH = 1
LOW = 0

INPUT_PULLDOWN = 4
INPUT_PULLUP = 2
OUTPUT = 1
INPUT = 0

# A0 = 14
# A1 = 15
# A2 = 16
# A3 = 17
# A4 = 18
# A5 = 19

class PyDuino:
    def __init__(self, address, conn, name):
        self.address = address
        self.conn = conn
        self.name = name

    @staticmethod
    def FindAll():
        # Get local IP address and subnet
        ip = socket.gethostbyname(socket.gethostname())
        subnet = ip[:ip.rfind('.')+1]

        # Create list of IP addresses in subnet
        candidates = [subnet + str(i) for i in range(1, 16)]

        # Get list of available COM ports and add them to candidates
        ports = [port.device for port in serial.tools.list_ports.comports()]
        candidates += ports

        # Validate candidates using multithreading and a thread-safe queue
        available = queue.Queue()
        threads = []
        for candidate in candidates:
            thread = threading.Thread(target=PyDuino._validate, args=(candidate, available))
            threads.append(thread)
            thread.start()

        for thread in threads:
            thread.join()

        # Convert the queue to a list and return it
        assoc = {}
        for d in available.queue:
            assoc[d.name] = d
        return assoc

    @staticmethod
    def _validate(candidate, available):
        # Validate IP addresses
        if '.' in candidate:
            try:
                # Connect to target and validate handshake response
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(TIMEOUT)
                sock.connect((candidate, 8266))
                pyduino = PyDuino(candidate, sock, None)
                data = pyduino._sendAndReceive('Handshake')
                if data.startswith('Ok'):
                    pyduino.name = data.split(',', 1)[1]
                    available.put(pyduino)
            except socket.timeout:
                pass

        # Validate COM ports
        else:
            try:
                # Connect to target and validate handshake response
                ser = serial.Serial(candidate, 115200, timeout=TIMEOUT)
                time.sleep(2)   # Really important!
                pyduino = PyDuino(candidate, ser, None)
                data = pyduino._sendAndReceive('Handshake')
                if data.startswith('Ok'):
                    pyduino.name = data.split(',', 1)[1]
                    available.put(pyduino)
            except (FileNotFoundError, serial.SerialException):
                pass

    def _sendAndReceive(self, message):
        bytestring = message + '\r\n'
        bytestring = bytestring.encode("utf-8")

        if isinstance(self.conn, serial.Serial):
            self.conn.flushInput()

            self.conn.write(bytestring)

            data = b''
            while not data.endswith(b'\r\n'):
                chunk = self.conn.read()
                if not chunk:
                    # Connection closed
                    break
                data += chunk
            data = data.strip().decode()
            return data

        if isinstance(self.conn, socket.socket):
            self.conn.setblocking(False)
            try:
                while True:
                    data = self.conn.recv(1)
                    if not data:
                        break
            except BlockingIOError:
                pass
            self.conn.setblocking(True)

            self.conn.send(bytestring)

            data = b''
            while not data.endswith(b'\r\n'):
                chunk = self.conn.recv(1)
                if not chunk:
                    # Connection closed
                    break
                data += chunk
            data = data.strip().decode()
            return data

    def pinMode(self, pin, value):
        r = self._sendAndReceive(f'pinMode,{pin},{value}')

    def digitalWrite(self, pin, value):
        r = self._sendAndReceive(f'digitalWrite,{pin},{value}')

    def digitalRead(self, pin):
        r = self._sendAndReceive(f'digitalRead,{pin}')
        return int(r)

    def analogWrite(self, pin, value):
        r = self._sendAndReceive(f'analogWrite,{pin},{value}')

    def analogRead(self, pin):
        r = self._sendAndReceive(f'analogRead,{pin}')
        return int(r)

