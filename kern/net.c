#include <kern/net.h>

#include <inc/stdio.h>
#include <inc/string.h>
#include <kern/e1000.h>
#include <kern/eth.h>
#include <kern/arp.h>
#include <kern/ip.h>
#include <kern/udp.h>

#define NET_LIMIT 1
#define BUF_SIZE  1024

void
net_serve() {

    // Managing frames receive from RX queue
    for (size_t i = 0; i < NET_LIMIT; ++i) {
        struct eth_pkt pkt;
        int len = eth_recv(&pkt);

        if (len > 0) {
            cprintf("\nNew ETH packet received: \n");
            cprintf("Length: %d \n", len);

            cprintf("Eth: Dst MAC: ");
            for (int i=0; i < 6; i++){
                cprintf("\\%x ", pkt.hdr.eth_dmac[i]);
            }
            cprintf("\n");

            cprintf("Eth: Src MAC: ");
            for (int i=0; i < 6; i++){
                cprintf("\\%x ", pkt.hdr.eth_smac[i]);
            }
            cprintf("\n");

            switch (pkt.hdr.eth_type){
            case ETH_TYPE_IPV4:
                cprintf("Eth: Type IPv4\n");
                ip_recv(&pkt, len);
                break;
            case ETH_TYPE_ARP:
                cprintf("Eth: Type ARP\n");
                arp_recv(&pkt);
                break;
            default:
                cprintf("Eth: Type Unknown: %x\n", pkt.hdr.eth_type);
            }
        } else {
            continue;
        }
    }

    // Sending of UDP packets
    for (size_t i = 0; i < 1; ++i) {
        char buf[6];
        memset(buf, 0xEE, 6);
        int res = udp_send(buf, 6);
        if (res < 0) {
            continue;
        }

    }
}
