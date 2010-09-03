/*
arp/arpint.{c,h} :
	Defines interrupt handler routines for the interrupts being used.
	The actual symbols for interrupt handlers are created in the startup script
	startup_stm32f10x_cl.s (in the CMSIS lib), because they must appear in
	predefined positions in the interrupt vector.
*/

#ifndef __ARP_INT__
#define __ARP_INT__

extern int cursamp;
extern int lowerrdy;
#endif