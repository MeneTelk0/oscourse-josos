#include <inc/stdio.h>
#include <inc/string.h>
#include <kern/e1000.h>
#include <kern/eth.h>
#include <kern/arp.h>

// IP known to the router - 192.168.56.101
uint8_t known_ip[4] = {0xc0, 0xa8, 0x38, 0x65};
uint8_t local_mac[6] = {0x52, 0x54, 0x0, 0x12, 0x34, 0x56};

int 
handle_arp_request(struct arp_hdr* arp_hdr){

    int cmp = memcmp(arp_hdr->target_ip, known_ip, sizeof(known_ip));
    if (cmp == 0) {
        cprintf("    ARP: Known Address: Sending Response \n");
        struct arp_hdr arp_reply_hdr;
        arp_reply_hdr.htype = arp_hdr->htype;
        arp_reply_hdr.ptype = arp_hdr->ptype;
        arp_reply_hdr.hlen = arp_hdr->hlen;
        arp_reply_hdr.plen = arp_hdr->plen;
        arp_reply_hdr.opcode = ARP_OP_REPLY;
        memcpy(arp_reply_hdr.target_mac, arp_hdr->sender_mac, sizeof(arp_hdr->sender_mac));
        memcpy(arp_reply_hdr.target_ip, arp_hdr->sender_ip, sizeof(arp_hdr->sender_ip));
        memcpy(arp_reply_hdr.sender_ip, known_ip, sizeof(known_ip));
        memcpy(arp_reply_hdr.sender_mac, local_mac, sizeof(local_mac));

        struct eth_hdr e_hdr;
        e_hdr.eth_type = ETH_TYPE_ARP;
        memcpy(e_hdr.eth_dmac, arp_reply_hdr.target_mac, sizeof(arp_reply_hdr.target_mac));

        return eth_send(&e_hdr, &arp_reply_hdr, sizeof(struct arp_hdr));
    } else {
        cprintf("    ARP: Ignoring Request: Unknown Address");
        return 0;
    }

    return 0;
}

int
arp_recv(struct eth_pkt* eth_pkt) {
    struct arp_hdr arp_hdr;
    memcpy(&arp_hdr, eth_pkt->data, sizeof(struct arp_hdr));

    cprintf("    ARP: Opcode: %x\n", arp_hdr.opcode);

    cprintf("    ARP: Sender MAC: ");
    for (int i=0; i < 6; i++){
        cprintf("\\%x ", arp_hdr.sender_mac[i]);
    }
    cprintf("\n");

    cprintf("    ARP: Sender IP: ");
    for (int i=0; i < 4; i++){
        cprintf("%d.", arp_hdr.sender_ip[i]);
    }
    cprintf("\n");

    cprintf("    ARP: Target MAC: ");
    for (int i=0; i < 6; i++){
        cprintf("\\%x ", arp_hdr.target_mac[i]);
    }
    cprintf("\n");

    cprintf("    ARP: Target IP: ");
    for (int i=0; i < 4; i++){
        cprintf("%d.", arp_hdr.target_ip[i]);
    }
    cprintf("\n");

    switch (arp_hdr.opcode){
    case ARP_OP_REQUEST:
        cprintf("    ARP: Opcode Request\n");
        handle_arp_request(&arp_hdr);
        break;
    default:
        cprintf("    ARP: Unknown Opcode\n");
        break;
    }

    return 0;
}