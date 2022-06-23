#ifndef JOS_KERN_IP_H
#define JOS_KERN_IP_H

#include <inc/types.h>
#include <kern/e1000.h>
#include <kern/eth.h>

struct ip_hdr {
    uint8_t ip_verlen;       // 4-bit IPv4 version 4-bit header length (in 32-bit words)
    uint8_t ip_tos;          // IP type of service
    uint16_t ip_totallength; // Total length
    uint16_t ip_id;          // Unique identifier
    uint16_t ip_offset;      // Fragment offset field
    uint8_t ip_ttl;          // Time to live
    uint8_t ip_protocol;     // Protocol(TCP,UDP etc)
    uint16_t ip_checksum;    // IP checksum
    uint32_t ip_srcaddr;     // Source address
    uint32_t ip_destaddr;    // Source address
} __attribute__((packed));


#define MTU            1500 // tmp magic
#define IP_HEADER_LEN  sizeof(struct ip_hdr)
#define ETH_HEADER_LEN sizeof(struct eth_hdr)
#define IP_DATA_LEN    (MTU - IP_HEADER_LEN - ETH_HEADER_LEN)

struct ip_pkt {
    struct ip_hdr hdr;
    uint8_t data[IP_DATA_LEN];
};

uint16_t ip_checksum(void* vdata, size_t length);
int ip_send(struct ip_pkt* pkt, uint16_t length);
int ip_recv(struct eth_pkt* eth_pkt, int length);

#define IP_VER 0x4
//#define IP_HLEN 0x05
#define IP_HLEN    (IP_HEADER_LEN / sizeof(uint32_t))
#define IP_VER_LEN (IP_VER << 4 | IP_HLEN)
#define IP_TTL     10

#define IPH_V(hdr)  ((hdr)->ip_verlen & 0xf)
#define IPH_HL(hdr) (((hdr)->ip_verlen >> 4) & 0x0f)

#define IP_PROTO_ICMP 1
#define IP_PROTO_UDP  17

#endif /* !JOS_KERN_IP_H */
