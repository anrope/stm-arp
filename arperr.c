#include <stm32f10x.h>
#include "arperr.h"

// int * errorbuf;
int errorbuf[ERRORBUFLEN];
int erroridx = 0;

void initerror(void)
{
	int i;
	
// 	errorbuf = (int *)malloc(sizeof(int)*ERRORBUFLEN);
	
	for (i=0; i<ERRORBUFLEN; i++)
		errorbuf[i] = 0;
	
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
}

void flagerror(int errorcode)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	
	errorbuf[erroridx] = errorcode;
	
	erroridx++;
	
	if (erroridx == ERRORBUFLEN)
		erroridx = 0;
}