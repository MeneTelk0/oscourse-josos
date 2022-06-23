#include <kern/ip.h>
#include <inc/string.h>
#include <kern/inet.h>
#include <inc/error.h>
#include <kern/eth.h>
#include <inc/stdio.h>
#include <kern/udp.h>


uint16_t packet_id = 0;
uint8_t target_mac[6] = {0xee, 0x6e, 0x6a, 0x22, 0x84, 0x00};

// QEMU IPv4 addr 192.168.56.101
uint32_t qemu_ipv4_addr = 3232249957;
// Destination IPv4 addr 192.168.56.1
uint32_t dest_ipv4_addr = 3232249857;

uint16_t
ip_checksum(void* vdata, size_t length) {
    char* data = vdata;
    uint32_t acc = 0xffff;
    for (size_t i = 0; i + 1 < length; i += 2) {
        uint16_t word;
        memcpy(&word, data + i, 2);
        acc += ntohs(word);
        if (acc > 0xffff) {
            acc -= 0xffff;
        }
    }
    // Handle any partial block at the end of the data.
    if (length & 1) {
        uint16_t word = 0;
        memcpy(&word, data + length - 1, 1);
        acc += ntohs(word);
        if (acc > 0xffff) {
            acc -= 0xffff;
        }
    }
    // Return the checksum in network byte order.
    return htons(~acc);
}

int
ip_send(struct ip_pkt* pkt, uint16_t length) {
    uint16_t id = packet_id++;

    // Construct IPv4 header
    struct ip_hdr* hdr = &pkt->hdr;
    hdr->ip_verlen = IP_VER_LEN;
    hdr->ip_tos = 0;
    hdr->ip_totallength = JHTONS(length + IP_HEADER_LEN);
    hdr->ip_id = id;
    hdr->ip_offset = 0; // TODO fragmentation
    //hdr->ip_protocol = 17;
    hdr->ip_ttl = IP_TTL;
    hdr->ip_destaddr = JHTONL(dest_ipv4_addr);
    hdr->ip_srcaddr = JHTONL(qemu_ipv4_addr);
    hdr->ip_checksum = ip_checksum((void*)pkt, IP_HEADER_LEN);

    // Construct Ethernet header
    struct eth_hdr e_hdr;
    e_hdr.eth_type = ETH_TYPE_IPV4;
    memcpy(e_hdr.eth_dmac, target_mac, sizeof(target_mac));

    // Transmit the packet
    return eth_send(&e_hdr, (void*)pkt, sizeof(struct ip_hdr) + length);


    // struct eth_hdr e_hdr;
    // e_hdr.eth_type = 0x0008;
    // char hard_code[6] = {0x3a, 0xbe, 0x6d, 0xa0, 0xaf, 0x00};
    // memcpy(e_hdr.eth_dmac, hard_code, 6);
    // return eth_send(&e_hdr, (void*)pkt, sizeof(struct ip_hdr) + length);
}

int
ip_recv(struct eth_pkt* eth_pkt, int length) {

    struct ip_pkt ip_pkt;
    memcpy(&ip_pkt, eth_pkt->data, length - sizeof(struct eth_hdr));

    struct ip_hdr* hdr = &ip_pkt.hdr;
    if (hdr->ip_verlen != IP_VER_LEN) {
        return -E_UNS_VER;
    }
    uint16_t checksum = hdr->ip_checksum;
    hdr->ip_checksum = 0;
    if (checksum != ip_checksum(&ip_pkt, IP_HEADER_LEN)) {
        return -E_INV_CHS;
    }

    cprintf("IPv4: Checksum: %d \n", hdr->ip_checksum);
    cprintf("IPv4: TTL: %d \n", hdr->ip_ttl);
    cprintf("IPv4: Src address: %ld \n", JHTONL(hdr->ip_srcaddr));
    cprintf("IPv4: Dst address: %ld \n", JHTONL(hdr->ip_destaddr));

    switch (hdr->ip_protocol){
    case IP_PROTO_ICMP:
        cprintf("IPv4: ICMP protocol \n");
        break;
    case IP_PROTO_UDP:
        cprintf("IPv4: UDP protocol \n");
        udp_recv(&ip_pkt);
        break;
    default:
        cprintf("IPv4: Unknown protocol \n");

    }

    return 0;
}
