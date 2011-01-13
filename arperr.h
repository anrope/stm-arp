/*
arp/arperr.{c,h} :
	Sets up a global error (circular) buffer. When flagerror() is called, a value
	corresponding to that error is pushed into the buffer. The header file contains
	the list of #define's that match error numbers with their errors. When an error
	occurs, the LED on port C, pin 9 will light up, and remain lit.
*/

#ifndef __ARP_ERR__
#define __ARP_ERR__

#define ERRORBUFLEN 100

//error list
#define HSE_START_FAIL 1
#define SAMPLE_OVERRUN_LOWER 2
#define SAMPLE_OVERRUN_UPPER 3
#define DAC_CONFIG_ERROR 4
#define ADC_CONFIG_ERROR 5

#define DEBUG_ERROR 13

/*
	initerror() is called once, on boot up, to set up
	the global error buffer
*/
void initerror(void);

/*
	flagerror() is called when an error occurs.
	errorcode is written to the error buffer.
*/
void flagerror(int errorcode);
#endif