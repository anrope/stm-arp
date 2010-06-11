#include <stdlib.h>
#include <stm32f10x.h>
#include "arpsample.h"
#include "arpfirblock.h"

uint32_t masked;
int shifted;

firinfo * init_fir(int * coefs, int ncoefs, int skip, int skipoff)
{
	int i;
	firinfo * filter;
	
	filter = (firinfo *)malloc(sizeof(firinfo));
	
	filter->ncoefs = ncoefs;
	filter->skip = skip;
	filter->skipct = skip-skipoff;
	
	filter->coefs = (int *)malloc(ncoefs * sizeof(int));
	for (i=0; i<ncoefs; i++)
		filter->coefs[i] = coefs[ncoefs-1-i];
	
	filter->history = (int *)malloc(ncoefs * sizeof(int));
	for (i=0; i<ncoefs; i++)
		filter->history[i] = 0;
	
	filter->index = 0;
	
	return filter;
}

// void calc_fir(firinfo * f, sampleblock * sb)
void calc_fir(firinfo * f, int * inbuf, int * outbuf, int nsamp)
{
	int i, j;
	int y;
	int * hitr;

	for (j=0; j<nsamp; j++)
	{		
		f->history[f->index] = inbuf[j];
		f->index++;
		if (f->index == f->ncoefs)
			f->index = 0;
		
		y = 0;
		
		hitr = f->coefs;

		for (i=f->index; i<f->ncoefs; i++)
			y += *(hitr++) * f->history[i];
		
		for (i=0; i<f->index; i++)
			y += *(hitr++) * f->history[i];

// 		sb->outsamp[sb->cursamp-1] = (uint16_t)((int32_t)filtout ^ (int32_t)0xffff8000);

		outbuf[j] = (y>>FIR_FBITS);

		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	}
		
	return;
}

void destroy_fir(firinfo * filter)
{
	free(filter->coefs);
	free(filter->history);
	free(filter);
}