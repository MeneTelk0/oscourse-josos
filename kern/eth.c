#include <kern/e1000.h>
#include <kern/eth.h>
#include <inc/string.h>
#include <kern/inet.h>
#include <inc/error.h>
#include <inc/assert.h>

//52:54:00:12:34:56
#define ETH_MAX_PACKET_SIZE 2048


char qemu_mac[6] = {0x52, 0x54, 0x0, 0x12, 0x34, 0x56};

int
eth_send(struct eth_hdr* hdr, void* data, size_t len) {
    assert(len <= ETH_MAX_PACKET_SIZE - sizeof(struct eth_hdr));
    memcpy((void*)hdr->eth_smac, qemu_mac, sizeof(hdr->eth_smac));

    char buf[ETH_MAX_PACKET_SIZE];

    hdr->eth_type = htons(hdr->eth_type);
    memcpy((void*)buf, (void*)hdr, sizeof(struct eth_hdr));

    memcpy((void*)buf + sizeof(struct eth_hdr), data, len);

    return tx_packet(buf, len + sizeof(struct eth_hdr));
}


int
eth_recv(struct eth_pkt* pkt) {

    int size = rx_packet((void*)pkt);
    if (size < 0) return size;

    return size;
}
