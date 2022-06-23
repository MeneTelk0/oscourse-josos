#include <kern/icmp.h>
#include <kern/inet.h>
#include <inc/string.h>
#include <inc/stdio.h>
#include <inc/assert.h>

// From here: https://github.com/mtcp-stack/mtcp/blob/master/mtcp/src/icmp.c
static uint16_t
ICMPChecksum(uint16_t *icmph, int len)
{
	assert(len >= 0);
	
	uint16_t ret = 0;
	uint32_t sum = 0;
	uint16_t odd_byte;
	
	while (len > 1) {
		sum += *icmph++;
		len -= 2;
	}
	
	if (len == 1) {
		*(uint8_t*)(&odd_byte) = * (uint8_t*)icmph;
		sum += odd_byte;
	}
	
	sum =  (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	ret =  ~sum;
	
	return ret; 
}

int 
handle_icmp_request(struct icmp_pkt* icmp_pkt){
    struct icmp_pkt icmp_reply_pkt;
    struct icmp_hdr* reply_hdr = &icmp_reply_pkt.hdr;
    struct icmp_hdr* hdr = &icmp_pkt->hdr;

    reply_hdr->icmp_type = ICMP_ECHO_REPLY;
    reply_hdr->icmp_code = 0;
    reply_hdr->un.echo.icmp_id = hdr->un.echo.icmp_id;
    reply_hdr->un.echo.icmp_sequence = hdr->un.echo.icmp_sequence;
    reply_hdr->icmp_checksum = 0;

    reply_hdr->icmp_checksum = ICMPChecksum((uint16_t *)reply_hdr, ICMP_HEADER_LEN);

    struct ip_pkt result;
    result.hdr.ip_protocol = IP_PROTO_ICMP;
    memcpy((void*)result.data, (void*)&icmp_reply_pkt, sizeof(struct icmp_hdr));

    return ip_send(&result, sizeof(struct icmp_hdr));
}

int
icmp_recv(struct ip_pkt* ip_pkt) {
    struct icmp_pkt icmp_pkt;
    int size = JNTOHS(ip_pkt->hdr.ip_totallength) - IP_HEADER_LEN;
    memcpy(&icmp_pkt, ip_pkt->data, size);
    struct icmp_hdr* hdr = &icmp_pkt.hdr;
    cprintf("ICMP: Type: %d\n", hdr->icmp_type);
    cprintf("ICMP: Code: %d\n", hdr->icmp_code);
    cprintf("ICMP: Checksum: %d\n", hdr->icmp_checksum);
    cprintf("ICMP: Identifier: %d\n", hdr->un.echo.icmp_id);
    cprintf("ICMP: Seq number: %d\n", hdr->un.echo.icmp_sequence);

    switch (hdr->icmp_type){
    case ICMP_ECHO_REPLY:
        cprintf("ICMP: Received ICMP ECHO Reply\n");
        break;
    case ICMP_ECHO_REQUEST:
        cprintf("ICMP: Received ICMP ECHO Request: Sending Reply\n");
        handle_icmp_request(&icmp_pkt);
        break;
    default:
        cprintf("ICMP: Received ICMP Unknown type\n");
        break;
    }
    return 0;
}