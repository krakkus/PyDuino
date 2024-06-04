import time
import socket
import serial
import serial.tools.list_ports
import threading
import queue

TIMEOUT = 1

HIGH = 1
LOW = 0

INPUT_PULLDOWN = 4
INPUT_PULLUP = 2
OUTPUT = 1
INPUT = 0


class PyDuino:
    def __init__(self, address, conn, name):
        self.address = address
        self.conn = conn
        self.name = name

    @staticmethod
    def FindAll(target_ip=None):
        all_ips = []

        # Get all network interfaces
        for interface in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET):
            address = interface[4][0]

            if not address.startswith('fe80:'):
                # Extract IP address and subnet
                ip = address.split('.')[0:3]  # Get first three parts for subnet
                subnet = '.'.join(ip) + '.'

                # Create list of candidates for this subnet
                candidates = [subnet + str(i) for i in range(1, 255)]
                all_ips.extend(candidates)

        # Add candidates from target subnet (if provided)
        if target_ip:
            target_subnet = target_ip.split('.')[0:3]
            target_subnet = '.'.join(target_subnet) + '.'
            target_candidates = [target_subnet + str(i) for i in range(1, 255)]
            all_ips.extend(target_candidates)

        # Get list of available COM ports and add them to candidates
        ports = [port.device for port in serial.tools.list_ports.comports()]
        all_ips.extend(ports)

        # Validate candidates using multithreading and a thread-safe queue
        available = queue.Queue()
        threads = []
        for candidate in all_ips:
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

    def servoWrite(self, pin, value):
        r = self._sendAndReceive(f'servoWrite,{pin},{value}')

    def stepperWrite_1(self, pin1, pin2, pin3, pin4, steps, sleep):
        r = self._sendAndReceive(f'stepperWrite_1,{pin1},{pin2},{pin3},{pin4},{steps},{sleep}')

    def stepperWrite_2(self, pin1, pin2, pin3, pin4, pin5, steps, sleep):
        r = self._sendAndReceive(f'stepperWrite_2,{pin1},{pin2},{pin3},{pin4},{pin5},{steps},{sleep}')


def map_range(sample_1_x, sample_1_y, sample_2_x, sample_2_y, my_x):
  """
  Maps a value from one range to another defined by two data points.

  Args:
      sample_1_x: The x-value of the first data point.
      sample_1_y: The y-value of the first data point.
      sample_2_x: The x-value of the second data point.
      sample_2_y: The y-value of the second data point.
      my_x: The value to be mapped.

  Returns:
      The mapped value within the new range.
  """

  # Avoid division by zero
  if sample_1_x == sample_2_x:
    return sample_1_y

  # Calculate the slope and offset
  slope = (sample_2_y - sample_1_y) / (sample_2_x - sample_1_x)
  offset = sample_1_y - (slope * sample_1_x)

  # Apply linear interpolation
  return (slope * my_x) + offset