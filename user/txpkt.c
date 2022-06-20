#include <inc/lib.h>
#include <inc/stdio.h>
#include <inc/string.h>

void umain(int argc, char **argv)
{
	char *data = "Hello world from user space";
	int err = sys_transmit_packet(data, strlen(data));
    cprintf("Err value %d \n", err);
	cprintf("packet of size %ld and content: %s has been transmitted\n",
	    strlen(data), data);
}