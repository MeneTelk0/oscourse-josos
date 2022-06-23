 #!/usr/bin/env python3

#******************************************************************
#
# Institute for System Programming of the Russian Academy of Sciences
# Copyright (C) 2022 ISPRAS
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, Version 3.
#
# This program is distributed in the hope # that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the GNU General Public License version 3 for more details.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

import argparse

parser = argparse.ArgumentParser()

parser.add_argument('--src-mac', help='Source (external) MAC address', default='ee:6e:6a:22:84:00')
parser.add_argument('--src-ip', help='Source (external) IP address', default='192.168.56.1')
parser.add_argument('--src-port', help='Source (external) port', type=int, default=10003)
parser.add_argument('--dst-mac', help='Destination (RTOS) MAC address', default='52:54:00:12:34:56')
parser.add_argument('--dst-ip', help='Destination (RTOS) IP address', default='192.168.56.101')
parser.add_argument('--dst-port', help='Destination (Unix) port', type=int, default=10002)
parser.add_argument('--mcast-ip', help='Multicast (QEMU) IP address', default='230.0.0.1')
parser.add_argument('--mcast-port', help='Multicast (QEMU) port', type=int, default=1234)
parser.add_argument('--proxy-port', help='Proxy port for external connections', type=int, default=1222)
parser.add_argument('--max-packet-size', help='Maximum packet size', type=int, default=65535)

args = parser.parse_args()

import socket
import struct
import scapy.all as scapy

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
sock.bind(('0.0.0.0', args.mcast_port))
mreq = struct.pack('4sL', socket.inet_aton(args.mcast_ip), socket.INADDR_ANY)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

def create_frame(src_mac, dst_mac, src_ip, dst_ip, src_udp_port, dst_udp_port, message_bytes):
    ether = scapy.Ether(src=src_mac, dst=dst_mac)
    ip = scapy.IP(src=src_ip, dst=dst_ip)
    udp = scapy.UDP(sport=src_udp_port, dport=dst_udp_port)
    payload = scapy.raw(message_bytes)
    whole_packet = ether / ip / udp / payload
    return scapy.raw(whole_packet)

def create_arp(src_eth_mac, src_mac, psrc, pdst):
    ether = scapy.Ether(src=src_eth_mac, dst="ff:ff:ff:ff:ff:ff")
    arp = scapy.ARP(psrc=psrc, pdst=pdst, hwsrc=src_mac)
    whole_packet = ether / arp
    return scapy.raw(whole_packet)

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('0.0.0.0', args.proxy_port))
server_socket.listen(5)

print(f'Listening on {args.proxy_port}...')
print(f'HINT: Run `nc localhost {args.proxy_port}` in terminal.')
client_socket, addr = server_socket.accept()
print(f'Connected!')

try:
    while True:
        try:
            data = sock.recv(args.max_packet_size, socket.MSG_DONTWAIT)
            if data:
                print("data 1", data)
                # eth_pkt = scapy.Ether(data)
                # ip_pkt = eth_pkt.payload
                # udp_pkt = ip_pkt.payload

                # if ip_pkt.dst == args.src_ip and udp_pkt.dport == args.src_port:
                #     client_socket.send(bytes(udp_pkt.payload))
        except BlockingIOError:
            pass

        try:
            data = client_socket.recv(1024, socket.MSG_DONTWAIT)
            if data:
                print("data 2", data)
                frame = create_frame(src_mac=args.src_mac,
                                     dst_mac=args.dst_mac,
                                     src_ip=args.src_ip,
                                     dst_ip=args.dst_ip,
                                     src_udp_port=args.src_port,
                                     dst_udp_port=args.dst_port,
                                     message_bytes=data)
                print("ETH frame: ", frame)
                err = sock.sendto(frame, (args.mcast_ip, args.mcast_port))
                print("Message length: ", err)

                # Send an ARP Request
                arp = create_arp(
                    src_eth_mac=args.src_mac,
                    src_mac=args.src_mac,
                    psrc=args.src_ip,
                    pdst=args.dst_ip
                )
                print("ARP frame: ", arp)
                err = sock.sendto(arp, (args.mcast_ip, args.mcast_port))
                print("Message length: ", err)

        except BlockingIOError:
            pass

except KeyboardInterrupt:
    print('Interrupted...')
    server_socket.close()
