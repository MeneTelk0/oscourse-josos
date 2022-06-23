#ifndef JOS_KERN_ETH_H
#define JOS_KERN_ETH_H

#include <inc/types.h>
#include <kern/e1000.h>

struct eth_hdr {
    uint8_t eth_dmac[6];
    uint8_t eth_smac[6];
    uint16_t eth_type;
} __attribute__((packed));

#define MTU            1500 // tmp magic
#define ETH_HEADER_LEN sizeof(struct eth_hdr)
#define ETH_DATA_LEN    (MTU - ETH_HEADER_LEN)

struct eth_pkt {
    struct eth_hdr hdr;
    uint8_t data[ETH_DATA_LEN];
};

int eth_send(struct eth_hdr* hdr, void* data, size_t len);
int eth_recv(struct eth_pkt* pkt);

extern char qemu_mac[];

#define ETH_TYPE_ARP 0x0608  //0x0806
#define ETH_TYPE_IPV4 0x0008 //0x0800


#endif /* !JOS_KERN_ETH_H */
