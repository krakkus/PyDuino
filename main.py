import ipaddress
import socket
import threading
import requests
import time

def check_ip(ip, results):
    try:
        response = requests.get(f"http://{ip}/api_discover.html", timeout=5)
        if response.status_code == 200:
            response_text = response.text.strip()
            if response_text.startswith("OK,"):
                device_id = response_text.split(",")[1]
                results.append((ip, device_id))
    except requests.exceptions.RequestException as e:
        pass
        #print(f"Error checking IP {ip}: {e}")

def check_ips(ip_list):
    results = []
    threads = []

    for ip in ip_list:
        thread = threading.Thread(target=check_ip, args=(ip, results))
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

    return results

def get_local_ipv4_addresses():
    try:
        import netifaces

        # Use netifaces for more detailed interface information
        interfaces = netifaces.interfaces()
        ip_addresses = []
        for interface in interfaces:
            addresses = netifaces.ifaddresses(interface)
            if netifaces.AF_INET in addresses:
                for addr in addresses[netifaces.AF_INET]:
                    if 'addr' in addr:
                        ip_addresses.append(addr['addr'])
        return ip_addresses

    except ImportError:
        # Fallback to socket if netifaces is not available
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))  # Connect to a public DNS server
        local_ip = s.getsockname()[0]
        s.close()
        return [local_ip]

def create_candidate_list(ip_addresses):
    candidates = []
    for ip in ip_addresses:
        left, right = split_ipv4(ip)
        for i in range(1, 256):
            if str(i) == right:
                continue
            candidates.append(left + str(i))

    return candidates

def split_ipv4(ip_address):
  ip = ipaddress.ip_address(ip_address)
  octets = str(ip).split(".")
  return ".".join(octets[:3]) + ".", octets[3]


if __name__ == "__main__":
    ip_addresses = get_local_ipv4_addresses()
    candidates = create_candidate_list(ip_addresses)

    results = check_ips(candidates)
    print(results)

