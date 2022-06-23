#include <kern/e1000.h>
#include <kern/pci.h>
#include <kern/pmap.h>
#include <inc/types.h>
#include <inc/string.h>
#include <inc/error.h>

// LAB 6: Your driver code here

struct tx_desc tx_desc_table[NU_DESC];
struct rx_desc rx_desc_table[NU_DESC];
char tx_buf[NU_DESC][BUFFER_SIZE];
char rx_buf[NU_DESC][BUFFER_SIZE];
volatile uint32_t *phy_mmio_addr;

int
e1000_attach(struct pci_func* pciFunction) {

    cprintf("Attaching E1000 \n");

    pci_func_enable(pciFunction);
    phy_mmio_addr = mmio_map_region(pciFunction->reg_base[0], pciFunction->reg_size[0]);

    char* base_addr = (char*)phy_mmio_addr;

    //Reset the device
    // uint32_t* ims = (uint32_t*)(base_addr + E1000_IMS);
    // *ims = 0; // disable interrupts
    // uint32_t* ctl = (uint32_t*)(base_addr + E1000_CTL);
    // *ctl |= E1000_CTL_RST;
    // *ims = 0; // redisable interrupts

    //base adress LOW
    uint32_t* tdbal = (uint32_t*)(base_addr + E1000_TDBAL);
    *tdbal = (uint64_t)PADDR(tx_desc_table);

    //base address HIGH
    uint32_t* tdbah = (uint32_t*)(base_addr + E1000_TDBAH);
    *tdbah = 0;

    //LENGTH
    uint32_t* tdlen = (uint32_t*)(base_addr + E1000_TDLEN);
    *tdlen = sizeof(tx_desc_table);

    // head
    uint32_t* tdh = (uint32_t*)(base_addr + E1000_TDH);
    *tdh = 0;

    // tail
    uint32_t* tdt = (uint32_t*)(base_addr + E1000_TDT);
    *tdt = 0;

    // TCTL (tranmit control) E1000_TCTL_en | E1000_TCTL_psp | E1000_TCTL_ct | E1000_TCTL_cold
    uint32_t* tctl = (uint32_t*)(base_addr + E1000_TCTL);
    *tctl = E1000_TCTL_EN | E1000_TCTL_PSP | E1000_TCTL_CT | E1000_TCTL_COLD;

    uint32_t* tipg = (uint32_t*)(base_addr + E1000_TIPG);
    *tipg = 0x60200a;

    int i;
    for (i = 0; i < NU_DESC; i++) {
        tx_desc_table[i].status |= 1;
        tx_desc_table[i].buf_addr = PADDR(tx_buf[i]);
    }


    //Setting up Receiver

    for (i = 0; i < NU_DESC; i++) {
        rx_desc_table[i].buf_addr = PADDR(rx_buf[i]);
    }

    //LOW
    uint32_t* rxral = (uint32_t*)(base_addr + E1000_RX_RAL);
    *rxral = 0x12005452;

    //HIGH
    uint32_t* rxrah = (uint32_t*)(base_addr + E1000_RX_RAH);
    *rxrah = 0x80005634;

    // cprintf("mac addr %u:%u", *(uint32_t*)(base_addr+E1000_RX_RAL), *(uint32_t*)(base_addr+E1000_RX_RAH));

    //MTA
    uint32_t* mta = (uint32_t*)(base_addr + E1000_MTA);
    *mta = 0;

    //Disable interrupts
    uint32_t* ims = (uint32_t*)(base_addr + E1000_IMS);
    *ims = 0;

    uint32_t* rdbal = (uint32_t*)(base_addr + E1000_RDBAL);
    *rdbal = (uint64_t)PADDR(rx_desc_table);

    uint32_t* rdbah = (uint32_t*)(base_addr + E1000_RDBAH);
    *rdbah = 0;

    //Length
    uint32_t* rdlen = (uint32_t*)(base_addr + E1000_RDLEN);
    *rdlen = sizeof(rx_desc_table);

    //head
    uint32_t* rdh = (uint32_t*)(base_addr + E1000_RDH);
    *rdh = 1;

    //tail
    uint32_t* rdt = (uint32_t*)(base_addr + E1000_RDT);
    *rdt = 0;

    //RCTL E1000_RCTL_en | E1000_RCTL_bam | E1000_RCTL_crc
    uint32_t* rctl = (uint32_t*)(base_addr + E1000_RCTL);
    *rctl = E1000_RCTL_EN | // enable receiver
    E1000_RCTL_SZ_2048 | // set the buffer size to 2048 bytes
    E1000_RCTL_BAM | // enable broadcast
    E1000_RCTL_CRC; // | // strip CRC
    // E1000_RCTL_SBP | // store bad packet
    // E1000_RCTL_UPE | // unicast promisc enabled
    // E1000_RCTL_MPE;  // multicast promisc enabled

    // // ask e1000 for receive interrupts.
    // uint32_t* rdtr = (uint32_t*)(base_addr + E1000_RDTR);
    // *rdtr = 0;// interrupt after every received packet (no timer)
    // uint32_t* radv = (uint32_t*)(base_addr + E1000_RADV);
    // *radv = 0; // interrupt after every packet (no timer)
    // *ims = (1 << 7); // RXDW -- Receiver Descriptor Write Back

    return 1;
}


int
tx_packet(char* buffer, int length) {
    // free head
    static int head = 0;
    uint32_t* free_desc_addr = (uint32_t*)((char*)phy_mmio_addr + E1000_TDT);

    //status to transmit
    if (!(tx_desc_table[head].status & 0x1)) {
        cprintf("descriptor is full not able to transmit ");
        return -1;
    }

    //data
    memmove(tx_buf[head], buffer, length);
    //length
    tx_desc_table[head].length = length;

    // report status and end of packet
    tx_desc_table[head].cmd |= (E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP);
    //next free
    head = (head + 1) % NU_DESC;
    *free_desc_addr = head;

    return 0;
}

int
rx_packet(char* buffer) {
    //free head
    uint32_t* free_desc_addr = (uint32_t*)((char*)phy_mmio_addr + E1000_RDT);
    int head = (*free_desc_addr == NU_DESC - 1) ? (0) : (*free_desc_addr + 1);
    //status to receive
    if (!(rx_desc_table[head].status & 0x1)) {
        // cprintf("no data received");
        return -1;
    }
    cprintf("some data received \n");

    //data
    int len = rx_desc_table[head].length;
    memmove(buffer, rx_buf[head], len);

    // Move rx queue head and set current status to 0
    rx_desc_table[head].status = 0;
    *free_desc_addr = head;

    return len;
}
