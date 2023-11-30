import socket
import sys
import time
import os
import glob

# This router uniquely handles packet input, it needs to read in the input file. 


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

def write_to_file(path, packet_to_write, send_to_router=None):
    out_file = open(path, "a")
    if (send_to_router == None):
        out_file.write(packet_to_write + "\n")
    else:
        out_file.write(packet_to_write + " " + "to Router " + send_to_router + "\n")
    out_file.close()

files = glob.glob('./output/*')
for f in files:
    os.remove(f)

forwarding_table = read_csv('input/router_1_table.csv')
default_gateway_port = find_default_gateway(forwarding_table)
forwarding_table_with_range = generate_forwarding_table_with_range(forwarding_table)

packets_table = read_csv('input/packets.csv')

for packet in packets_table:
    sourceIP = packet[0]
    destinationIP = packet[1]
    payload = packet[2]
    ttl = int(packet[3])

    new_ttl = ttl - 1
    new_packet = str([sourceIP,destinationIP,payload,str(new_ttl)])

    destinationIP_bin = ip_to_bin(destinationIP)
    destinationIP_int = int(destinationIP_bin[2:],2)

    outPort = ""
    for row in forwarding_table_with_range:
        #print(str(row[0]) + " vs " + str(destinationIP_int) + " vs " + str(row[1]) + "\n")
        if (int(row[0]) <= destinationIP_int) and (destinationIP_int <= int(row[1])):
            #print("Output Port: " + str(row[3]) + "\n")
            outPort = (str(row[3])).strip()
            # print("Outport found, set to " + outPort)
            break
        

    if (outPort == ""):
        outPort = default_gateway_port
    
    #print("Dest: " + destinationIP)
    #print("Port: " + outPort)
    localIP = "127.0.0.1"
    
    # print("eval outport " + outPort + " with ttl " + str(new_ttl))

    if ((outPort == "8002") and (new_ttl > 0)):
        print("sending packet", new_packet, "to Router 2")
        write_to_file("output/sent_by_router_1.txt", new_packet, "2")
        soc = create_socket(localIP,int(outPort))
        soc.send(new_packet.encode())
        #clientData = soc.recv(4096)
        #print(clientData.decode())
    elif ((outPort == "8004") and (new_ttl > 0)):
        print("sending packet", new_packet, "to Router 4")
        write_to_file("output/sent_by_router_1.txt", new_packet, "4")
        soc = create_socket(localIP,int(outPort))
        soc.send(new_packet.encode())
        #clientData = soc.recv(4096)
        #print(clientData.decode())
    elif ((outPort == '127.0.0.1') and (new_ttl > 0)):
        print("OUT:", payload)
        write_to_file("output/out_router_1.txt", new_packet)
    else:
        print("DISCARD:", new_packet)
        write_to_file("output/discarded_router_1.txt", new_packet)

    time.sleep(1)
