#ifndef JOS_KERN_UDP_H
#define JOS_KERN_UDP_H

#include <inc/types.h>
#include <kern/ip.h>

#define UDP_DATA_LENGTH 1500


struct udp_hdr {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed));


struct udp_pkt {
    struct udp_hdr hdr;
    uint8_t data[UDP_DATA_LENGTH];
} __attribute__((packed));


int udp_send(void* data, int length);
int udp_recv(struct ip_pkt* pkt);


#endif /* !JOS_KERN_UDP_H */
