#include <kern/net.h>

#include <inc/stdio.h>
#include <inc/string.h>
#include <kern/e1000.h>
#include <kern/eth.h>

#define NET_LIMIT 2
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
        int len = rx_packet(buf);
        // struct eth_hdr hdr;
        // char data;
        // int res = eth_recv(&hdr, &data);
        if (len > 0) {
            cprintf("Length: %d \n", len);
            cprintf("Received packet contents: \n");
            for (int i = 0; i < len; i++){
                cprintf("\\%X ", buf[i]);
            }
            cprintf("\n");
        }
        if (len < 0) {
            continue;
        }
        // cprintf("normal eth_recv\n");
    }
}
