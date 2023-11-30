import socket
import sys
import traceback
from threading import Thread
import string

def create_socket(host, port):
    soc = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    try:
        soc.connect((host,port))
    except:
        print("Connection Error to", port)
        sys.exit()
    return soc

def read_csv(path):
    table_file = open(path, "r")
    table = table_file.readlines()
    table_list = []
    for row in table:
        array = row.split(",")
        for item in array:
            item.strip()
        table_list.append(array)
    table_file.close()
    return table_list

def find_default_gateway(table):
    for row in table:
        if (row[0] == "0.0.0.0"):
            return row[3]

def generate_forwarding_table_with_range(table):
    new_table = []
    for row in table:
        if (row[0] != "0.0.0.0"):
            network_dst_string = row[0]
            netmask_string = row[1]
            network_dst_bin = ip_to_bin(network_dst_string)
            netmask_bin = ip_to_bin(netmask_string)
            ip_range = find_ip_range(network_dst_bin[2:],netmask_bin[2:])
            new_row = ip_range
            new_row.append(row[2])
            new_row.append(row[3])
            new_table.append(new_row)
    return new_table

def ip_to_bin(ip):
    ip_octets = ip.split(".")
    ip_bin_string = ""
    for octet in ip_octets:
        int_octet = int(octet)
        bin_octet = bin(int_octet)
        bin_octet_string = bin_octet[2:].zfill(8)
        ip_bin_string += bin_octet_string
    ip_int = int(ip_bin_string,2)
    return bin(ip_int)

def find_ip_range(network_dst, netmask):
    min_ip_bin = bit_and(network_dst, netmask)
    min_ip = int(min_ip_bin,2)
    rangeWidth = bit_not(int(netmask,2))
    max_ip = min_ip + rangeWidth
    return [min_ip, max_ip]

def bit_not(n, numbits=32):
    return (1 << numbits) - 1 - n

def bit_and(in1, in2):
    a = int(in1,2)
    b = int(in2,2)
    return bin(a & b)

def receive_packet(connection, max_buffer_size):
    received_packet = connection.recv(max_buffer_size)
    packet_size = sys.getsizeof(received_packet)
    if packet_size > max_buffer_size:
        print("The packet size is greater than expected", packet_size)
    decoded_packet = received_packet.decode('utf-8')
    print("received packet", decoded_packet)
    write_to_file("output/received_by_router_3.txt", decoded_packet, send_to_router=None)
    decoded_packet = decoded_packet.strip('][')
    decoded_packet = decoded_packet.replace("'","")
    packet = decoded_packet.split(', ')
    return packet

def write_to_file(path, packet_to_write, send_to_router=None):
    out_file = open(path, "a")
    if (send_to_router == None):
        out_file.write(packet_to_write + "\n")
    else:
        out_file.write(packet_to_write + " " + "to Router " + send_to_router + "\n")
    out_file.close()

def start_server():
    host = "127.0.0.1"
    port = 8003
    soc = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    soc.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print("Socket created")
    try:
        soc.bind((host,port))
    except:
        print("Bind failed. Error : " + str(sys.exc_info()))
        sys.exit()
    soc.listen(1)
    print("Socket now listening")

    forwarding_table = read_csv('input/router_3_table.csv')
    default_gateway_port = find_default_gateway(forwarding_table)
    forwarding_table_with_range = generate_forwarding_table_with_range(forwarding_table)

    while True:
        connection, address = soc.accept()
        ip = address[0]
        port = address[1]
        print("Connected with " + str(ip) + ":" + str(port))
        try:
            processing_thread(connection, ip, port, forwarding_table_with_range, default_gateway_port)
        except:
            print("Thread did not start.")
            traceback.print_exc()


def processing_thread(connection, ip, port, forwarding_table_with_range, default_gateway_port, max_buffer_size=5120):

    while True:
        packet = receive_packet(connection,max_buffer_size)

        if (not packet):
            break

        sourceIP = packet[0]
        destinationIP = packet[1]
        payload = packet[2]
        ttl = int(packet[3])

        new_ttl = ttl - 1
        new_packet = str([sourceIP,destinationIP,payload,new_ttl])

        destinationIP_bin = ip_to_bin(destinationIP)
        destinationIP_int = int(destinationIP_bin[2:],2)

        outPort = ""
        for row in forwarding_table_with_range:
            if (int(row[0]) <= destinationIP_int) and (destinationIP_int <= int(row[1])):
                outPort = (str(row[3])).strip()
                break

        if (outPort == ""):
            outPort = default_gateway_port

        if ((outPort == "127.0.0.1") and (new_ttl > 0)):
            print("OUT:", payload)
            write_to_file("output/out_router_3.txt", new_packet)
        else:
            print("DISCARD:", new_packet)
            write_to_file("output/discarded_router_3.txt", new_packet)

start_server()
