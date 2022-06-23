#ifndef JOS_KERN_ICMP_H
#define JOS_KERN_ICMP_H

#include <inc/types.h>
#include <kern/ip.h>

#define ICMP_DATA_LENGTH 1500


struct icmp_hdr {
    uint8_t  icmp_type;
	uint8_t  icmp_code;
	uint16_t icmp_checksum;
	union {
		struct {
			uint16_t icmp_id;
			uint16_t icmp_sequence;
		} echo;                     // ECHO | ECHOREPLY
		struct {
			uint16_t unused;
			uint16_t nhop_mtu;
		} dest;                     // DEST_UNREACH
	} un;
} __attribute__((packed));

struct icmp_pkt {
    struct icmp_hdr hdr;
    uint8_t data[ICMP_DATA_LENGTH];
} __attribute__((packed));

#define ICMP_HEADER_LEN  sizeof(struct icmp_hdr)

// int icmp_send(void* data, int length);
int icmp_recv(struct ip_pkt* pkt);

#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8


#endif /* !JOS_KERN_ICMP_H */