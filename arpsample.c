#include <stm32f10x.h>

#define ADCBUFLEN 100
#define DACBUFLEN 100

#define ADCWAIT 50

extern volatile int newsample;

volatile int16_t adcbuf[ADCBUFLEN];
volatile uint16_t dacbuf[DACBUFLEN];

int16_t adcsamp;

static int cursamp;

int lowerrdy;

int filtout;
uint32_t filtscaled;

int16_t getsample(void)
{
	if (cursamp == ADCWAIT)
	{
		if (!lowerrdy)
		{
			//error
		}
		
		//pin 5 high while waiting for upper half to be ready
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
		while (lowerrdy);
		//work on upper
	}
	
	if (cursamp == ADCBUFLEN)
	{
		if (lowerrdy)
		{
			//error
		}
		
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
		while (!lowerrdy);
		
		cursamp = 0;
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