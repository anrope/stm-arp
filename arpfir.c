#include "arpfir.h"

firinfo * init_fir(int * coefs, int ncoefs)
{
	int i;
	firinfo * filter;
	
	filter = (firinfo *)malloc(sizeof(firinfo));
	
	filter->ncoefs = ncoefs;
	
	filter->coefs = (int *)malloc(ncoefs * sizeof(int));
	for (i=0; i<ncoefs; i++)
		filter->coefs[i] = coefs[ncoefs-1-i];
	
	filter->history = (int *)malloc(ncoefs * sizeof(int));
	for (i=0; i<ncoefs; i++)
		filter->history[i] = 0;
	
	filter->index = 0;
	
	return filter;
}

int calc_fir(firinfo * f, int x)
{
	int i;
	int y;
	int * hitr
	
	f->history[f->index] = x;
	f->index++;
	if (f->index == f->ncoefs)
		f->index = 0;
	
	y = 0;
	
	hitr = f->coefs;
	
	for (i=f->index; i<f->ncoefs; i++)
		y += *(hitr++) * f->history[i];
	
	for (i=0; i<f->index; i++)
		y += *(hitr++) * f->history[i];
	
	return (y>>FIR_FBITS);
}

void destroy_fir(firinfo * filter)
{
	free(filter->coefs);
	free(filter->history);
	free(filter);
}