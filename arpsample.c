#include <stm32f10x.h>
#include "arpsample.h"
#include "arperr.h"

volatile int16_t adcbuf[ADCBUFLEN];
volatile uint16_t dacbuf[DACBUFLEN];

int16_t adcsamp;

static int cursamp;

volatile int lowerrdy;

int filtout;
uint32_t filtscaled;

int16_t getsample(void)
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

	adcsamp = adcbuf[cursamp] ^ 0x8000;
	
	cursamp++;
	
	return adcsamp;
}

void putsample(int32_t filtout)
{	
	filtscaled = filtout ^ 0xffff8000;
	
	dacbuf[cursamp-1] = filtscaled;
	
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
}