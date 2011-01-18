/*
arp/arpsample.{c,h} :
	Defines the interface that the user will use to handle data samples. There are
	routines for one sample at a time, as well as sample blocks. The interfaces
	provide samples sampled at 48 kHz, and expect to be able to output at 48 kHz.
*/

#include <stm32f10x.h>
#include <stdlib.h>
#include "arpsample.h"
#include "arperr.h"

volatile uint32_t adcbuf[ADCBUFLEN];
volatile uint32_t dacbuf[DACBUFLEN];

volatile uint32_t * inbuf = &(adcbuf[ADCWAIT]);
volatile uint32_t * outbuf;

int16_t adcsamp;
int insamp;

static int cursamp;

volatile int lowerrdy = 0;

int filtout;
uint32_t filtscaled;

/*
	getsample() returns a single sample to the user.

	This routine must be called often enough to prevent
	buffer overrun, with samples occuring at 48 kHz.

	In the event of an overrun, an error LED will come
	on (from flagerror()).

	Port C, pin 8 LED will be on while working on the
	lower half of the buffer.

	Port C, pin 5 LED will be on while working on the
	upper half of the buffer.

	When both pc8 and pc5 LEDs are off (high), the
	processor is idle.
*/
int32_t getsample(void)
{
	if (cursamp == ADCBUFLEN)
	{
		if (lowerrdy)
		{
			//error
			flagerror(SAMPLE_OVERRUN_LOWER);
		}
		
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		//waiting for lower half to be ready
		while (!lowerrdy);
		cursamp = 0;
		//work on lower
	}
	
	if (cursamp == ADCWAIT)
	{
		if (!lowerrdy)
		{
			//error
			flagerror(SAMPLE_OVERRUN_UPPER);
		}
		
		//pin 5 high while waiting for upper half to be ready
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
		while (lowerrdy);
		//work on upper
	}

// 	adcsamp = adcbuf[cursamp] ^ 0x8000;
	
	cursamp++;
	
// 	return adcsamp;
	return (int16_t)((int16_t)adcbuf[cursamp-1] ^ (uint16_t)0x8000);
}

/*
	putsample() takes a single sample from the user
	and places it in a buffer to be output by the
	DAC.
*/
void putsample(int32_t filtout)
{
	dacbuf[cursamp-1] = (uint16_t)((int32_t)filtout ^ (int32_t)0xffff8000);

	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}

/*
	Simple accessor to return block size.
	Needed for creating a working buffer.
*/
int getblocksize()
{
	return ADCWAIT;
}

/*
	getblock() is called by the user when the user is
	ready to process a new block of samples. The user
	provides a pointer to an already allocated buffer
	that will be filled with Q14 samples.
*/
void getblock(int * working)
{
	int i;

	if (inbuf == &(adcbuf[ADCWAIT]))
	{
		if (lowerrdy)
		{
			//error
			flagerror(SAMPLE_OVERRUN_LOWER);
		}
		
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		//waiting for lower half to be ready
		while (!lowerrdy);
		cursamp = 0;
		//work on lower
		inbuf = adcbuf;
	} else {
		if (!lowerrdy)
		{
			//error
			flagerror(SAMPLE_OVERRUN_UPPER);
		}
		
		//pin 5 high while waiting for upper half to be ready
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
		while (lowerrdy);
		//work on upper
		inbuf = &(adcbuf[ADCWAIT]);
	}

	//fill the user's buffer with properly converted
	//Q14 values
	for (i=0; i<ADCWAIT; i++)
	{
// 		working[i] = (int16_t)((int16_t)inbuf[i] ^ (uint16_t)0x8000);
		//TODO: need to mask this now that adcbuf is 32 bits?
		working[i] = ADCTOQ14(inbuf[i]);
	}
}

void getblockstereo(int * chan1, int * chan2)
{
	int i;

	if (inbuf == &(adcbuf[ADCWAIT]))
	{
		if (lowerrdy)
		{
			//error
			flagerror(SAMPLE_OVERRUN_LOWER);
		}
		
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		//waiting for lower half to be ready
		while (!lowerrdy);
		cursamp = 0;
		//work on lower
		inbuf = adcbuf;
	} else {
		if (!lowerrdy)
		{
			//error
			flagerror(SAMPLE_OVERRUN_UPPER);
		}
		
		//pin 5 high while waiting for upper half to be ready
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
		while (lowerrdy);
		//work on upper
		inbuf = &(adcbuf[ADCWAIT]);
	}

	//fill the user's buffer with properly converted
	//Q14 values
	for (i=0; i<ADCWAIT; i++)
	{
		chan1[i] = ADCTOQ14(inbuf[i] & 0x0000ffff);
		chan2[i] = ADCTOQ14((inbuf[i] >> 16) & 0x0000ffff);
	}
}


/*
	putblock() is called by the user when they've finished
	processing a block and are ready for the DAC to output.

	Handles converting the samples to a format the DAC
	likes, and copies the samples to the DAC DMA's buffer.
*/
void putblock(int * working)
{
	int i;

	if (inbuf == adcbuf)
	{
		outbuf = dacbuf;
	} else {
		outbuf = &(dacbuf[ADCWAIT]);
	}

	//prepare the samples for the DAC, and copy them
	//to the DAC DMA buffer
	for (i=0; i<ADCWAIT; i++)
	{
		outbuf[i] = Q14TODAC(working[i]);
	}

	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}

void putblockstereo(int * chan1, int * chan2)
{
	int i;

	if (inbuf == adcbuf)
	{
		outbuf = dacbuf;
	} else {
		outbuf = &(dacbuf[ADCWAIT]);
	}

	//prepare the samples for the DAC, and copy them
	//to the DAC DMA buffer
	for (i=0; i<ADCWAIT; i++)
	{
		outbuf[i] = ((Q14TODAC(chan2[i])<<16) & 0xffff0000) | (Q14TODAC(chan1[i]) & 0x0000ffff); 
	}

	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}
