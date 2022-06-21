#include <inc/lib.h>
#include <inc/stdio.h>
#include <inc/string.h>

void umain(int argc, char **argv)
{
	char buf[1518];
	int len;
	
	while(1)
	{
		len = sys_receive_packet(buf);
		if (len > 0) {
            cprintf("Packet contents %s \n", buf);
        }
	}
}