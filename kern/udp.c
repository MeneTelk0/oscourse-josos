#include <kern/udp.h>
#include <kern/inet.h>
#include <inc/string.h>
#include <inc/stdio.h>

int
udp_send(void* data, int length) {
    struct udp_pkt pkt;
    struct udp_hdr* hdr = &pkt.hdr;
    hdr->src_port = JHTONS(10002);
    hdr->dst_port = JHTONS(10003);
    hdr->length = JHTONS(length + sizeof(struct udp_hdr));
    hdr->checksum = 0;
    memcpy((void*)pkt.data, data, length);

    struct ip_pkt result;
    result.hdr.ip_protocol = IP_PROTO_UDP;
    memcpy((void*)result.data, (void*)&pkt, length + sizeof(struct udp_hdr));

    return ip_send(&result, length + sizeof(struct udp_hdr));
}

int
udp_recv(struct ip_pkt* ip_pkt) {
    struct udp_pkt udp_pkt;
    int size = JNTOHS(ip_pkt->hdr.ip_totallength) - IP_HEADER_LEN;
    memcpy(&udp_pkt, ip_pkt->data, size);
    struct udp_hdr* hdr = &udp_pkt.hdr;
    cprintf("UDP: Src port: %d\n", JNTOHS(hdr->src_port));
    cprintf("UDP: Dest port: %d\n", JNTOHS(hdr->dst_port));
    cprintf("UDP: Length: %d\n", JNTOHS(hdr->length));
    cprintf("UDP: Data: ");
    for (size_t i = 0; i < JNTOHS(hdr->length) - sizeof(struct udp_hdr); i++) {
        cprintf("%c", udp_pkt.data[i]);
    }
    return 0;
}
