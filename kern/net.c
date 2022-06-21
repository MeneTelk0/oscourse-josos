#include <kern/net.h>

#include <inc/stdio.h>
#include <inc/string.h>
#include <kern/e1000.h>
#include <kern/eth.h>

#define NET_LIMIT 20
#define BUF_SIZE  1024

void
net_serve() {
    for (size_t i = 0; i < NET_LIMIT; ++i) {
        // struct ip_pkt pkt;
        // int res = ip_recv(&pkt);
        // if (res < 0) {
        //     //cprintf("error in ip_recv\n");
        //     continue;
        // }
        
        char buf[BUFFER_SIZE];
        int res = rx_packet(buf);
        // struct eth_hdr hdr;
        // char data;
        // int res = eth_recv(&hdr, &data);
        if (res > 0) {
            cprintf("Packet contents %s \n", buf);
        }
        if (res < 0) {
            continue;
        }
        cprintf("normal eth_recv\n");
    }
}
