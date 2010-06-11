#include <stm32f10x.h>
#include <stdlib.h>
#include "arpsample.h"
#include "arperr.h"

volatile int16_t adcbuf[ADCBUFLEN];
volatile uint16_t dacbuf[DACBUFLEN];

volatile int16_t * inbuf = &(adcbuf[ADCWAIT]);
volatile uint16_t * outbuf;

int16_t adcsamp;
int insamp;

static int cursamp;

volatile int lowerrdy = 0;

int filtout;
uint32_t filtscaled;

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

void putsample(int32_t filtout)
{	
// 	filtscaled = filtout ^ 0xffff8000;
// 	
// 	dacbuf[cursamp-1] = filtscaled;

	dacbuf[cursamp-1] = (uint16_t)((int32_t)filtout ^ (int32_t)0xffff8000);

	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}

sampleblock * prepblock(void)
{
	static sampleblock sblock = {&(adcbuf[ADCWAIT]),
									&(dacbuf[ADCWAIT]),
									ADCWAIT,
									0};
	sampleblock * sb = &sblock;

	sb->cursamp = 0;
	sb->nsamp = ADCWAIT;

	if (sb->insamp == &(adcbuf[ADCWAIT]))
	{
		if (lowerrdy)
		{
			//error
			flagerror(SAMPLE_OVERRUN_LOWER);
		}
		
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		//waiting for lower half to be ready
		while (!lowerrdy);
		//work on lower
		sb->insamp = adcbuf;
		sb->outsamp = dacbuf;
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
		sb->insamp = &(adcbuf[ADCWAIT]);
		sb->outsamp = &(dacbuf[DACWAIT]);
	}
	
	return sb;
}

int getblocksize()
{
	return ADCWAIT;
}

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

	for (i=0; i<ADCWAIT; i++)
	{
		working[i] = (int16_t)((int16_t)inbuf[i] ^ (uint16_t)0x8000);
	}

// 	return (int16_t)((int16_t)adcbuf[cursamp-1] ^ (uint16_t)0x8000);
}

void putblock(int * working)
{
	int i;

// 	dacbuf[cursamp-1] = (uint16_t)((int32_t)filtout ^ (int32_t)0xffff8000);
	if (inbuf == adcbuf)
	{
		outbuf = dacbuf;
	} else {
		outbuf = &(dacbuf[ADCWAIT]);
	}

	for (i=0; i<ADCWAIT; i++)
	{
		outbuf[i] = (uint16_t)((int32_t)working[i] ^ (int32_t)0xffff8000);
	}

	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}

