#!/usr/bin/env python
import socket
import binascii

#UDP_PORT = 7
UDP_PORT = 9

MAC = "001b6399f5cd"

def magic_packet(mac):
    mp = "ffffffffffff"
    
    for i in range(16):
        mp += mac

    return binascii.unhexlify(mp)


if __name__ == "__main__":
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    #sock.sendto(magic_packet(MAC) , ('255.255.255.255', UDP_PORT))
    sock.sendto(magic_packet(MAC) , ('<broadcast>', UDP_PORT))
    
