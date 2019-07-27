import socket
import sys
import _thread
import json
import time

self_address = ""
self_port = 0
routing_table = {}      # 静态路由表

# 使用 UDP Socket
host_addr = '127.0.0.1'
server = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) 
client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

def setup():
    global self_address, self_port, routing_table
    config_path = sys.argv[1]
    with open(config_path, 'r') as f:
        lines = f.readlines()
        this_router = lines[0].split()
        self_address = this_router[0]
        self_port = int(this_router[1])
        lines = lines[1:]
        for line in lines:
            route = line.split()
            routing_table[route[0]] = int(route[1])
    server.bind((host_addr, self_port))
    print("Setup finish.")
    print("Address of this router: {0}".format(self_address))
    print("Address of this port: {0}".format(self_port))
    print("Routing table: {0}".format(routing_table))

def parse_ip_packet(data):
    data = data.decode()
    packet = json.loads(data)
    return packet["payload"], packet["source_addr"], packet["des_addr"]

def forward():
    # global self_address, self_port, host_addr, routing_table, server
    while True:
        data, addr=server.recvfrom(1024)
        payload, source, des = parse_ip_packet(data)
        print("Receive: " + data.decode())
        if des in routing_table:
            address = routing_table.get(des)
            print("address = " + str(address))
            server.sendto(data, (host_addr, address))
        time.sleep(0.01)

def send(content, des_addr):
    data = '{{"payload": "{0}", "source_addr": "{1}", "des_addr": "{2}"}}'.format(str(content), str(self_address), str(des_addr))
    data = data.encode()
    if des_addr in routing_table:
        address = routing_table.get(des_addr)  # port actually
        client.sendto(data, (host_addr, address))
    else:
        print("Invalid address.")

def user_input():
    # two type of command
    # 1. send [message3] [address] - used by user input
    # 2. set [in_IP] [out_IP]  - used by router configuration
    while 1:
        cmd = input(">>")
        parsed_cmd = cmd.split(' ')
        cmd_type = parsed_cmd[0]

        if cmd_type == "send":
            if(len(parsed_cmd) >= 3):
                content = parsed_cmd[1]
                address = parsed_cmd[2]
                send(content, address)
            else:
                print("Usage: send [message] [address]")
        elif cmd_type == "set":
            pass
        elif cmd_type == "help":
            print("Two type of command:\n \
            1. send [message] [address] - used by user input\n\
            2. set [in_IP] [out_IP]  - used by router configuration")
        else:
            print("Invalid command. Type help to look up available commands.")
        time.sleep(0.01)

if __name__ == "__main__":
    setup()
    try:
        _thread.start_new_thread(forward, ())
        _thread.start_new_thread(user_input, ())
    except:
        print("Error: 无法启动线程")

    while 1:
        pass