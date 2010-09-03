/*
arp/arperr.{c,h} :
	Sets up a global error (circular) buffer. When flagerror() is called, a value
	corresponding to that error is pushed into the buffer. The header file contains
	the list of #define's that match error numbers with their errors. When an error
	occurs, the LED on port C, pin 9 will light up, and remain lit.
*/

#include <stm32f10x.h>
#include "arperr.h"

int errorbuf[ERRORBUFLEN];
int erroridx = 0;

/*
	initerror() is called once, on boot up, to set up
	the global error buffer
*/
void initerror(void)
{
	int i;
	
	for (i=0; i<ERRORBUFLEN; i++)
		errorbuf[i] = 0;
	
	//turn off our error indicating LED
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
}

/*
	flagerror() is called when an error occurs.
	errorcode is written to the error buffer.
*/
void flagerror(int errorcode)
{
	//turn on our error indicating LED
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	
	errorbuf[erroridx] = errorcode;
	
	erroridx++;
	
	if (erroridx == ERRORBUFLEN)
		erroridx = 0;
}