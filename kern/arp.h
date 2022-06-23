#ifndef JOS_KERN_ARP_H
#define JOS_KERN_ARP_H

#include <inc/types.h>
#include <kern/eth.h>

// Define an struct for ARP header
struct arp_hdr {
  uint16_t htype; /* Format of hardware address */
  uint16_t ptype; /* Format of protocol address */
  uint8_t hlen;   /* Length of hardware address */
  uint8_t plen;   /* Length of protocol address */
  uint16_t opcode; /* ARP opcode (command) */
  uint8_t sender_mac[6]; /* Sender hardware address */
  uint8_t sender_ip[4]; /* Sender IP address */
  uint8_t target_mac[6]; /* Target hardware address */
  uint8_t target_ip[4];  /* Target IP address */
} __attribute__((packed));

int arp_recv(struct eth_pkt* eth_pkt);

#define ARP_OP_REQUEST 0x0100  //0x0001
#define ARP_OP_REPLY   0x0200  //0x0002

#endif // JOS_KERN_ARP_H