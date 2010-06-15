#include <stm32f10x.h>
#include <stdlib.h>
#include "biquadblock_filter.h"
#include "arpsample.h"

BIQUAD_STRUCT *init_biquad(
        int     sections,
        int     scale,
        int     a_coefs[][3],
        int     b_coefs[][3])
    {
    BIQUAD_STRUCT *s;
    int i;
    
    s = (BIQUAD_STRUCT *)malloc( sizeof(BIQUAD_STRUCT) );
    s->sections = sections;
    s->scale = scale;
    s->a = a_coefs;
    s->b = b_coefs;
    s->w = (int (*)[2]) malloc( sections * 2 * sizeof(int) );
    for (i=0; i<sections; i++)
        {
	s->w[i][0] = 0.0;
	s->w[i][1] = 0.0;
	}
    return(s);
    }
	
void calc_biquad( BIQUAD_STRUCT *s, int * inbuf, int * outbuf, int nsamp)
{
    int i, j;
    int w, x;
	
	for (j=0; j<nsamp; j++)
	{
		x = ADCTOQ14(inbuf[j]) * s->scale;
    	for (i=0; i<s->sections; i++)
		{
			w = (x - s->a[i][1] * s->w[i][0] - s->a[i][2] * s->w[i][1]) >> BIQUAD_FBITS ;
			x = s->b[i][0] * w + s->b[i][1] * s->w[i][0] + s->b[i][2] * s->w[i][1];
	
			s->w[i][1] = s->w[i][0];   /* Update the section history */
			s->w[i][0] = w;
		}
		outbuf[j] = Q14TODAC(x >> BIQUAD_FBITS);
	}
}

void destroy_biquad(BIQUAD_STRUCT *s)
    {
    free(s->w);
    free(s);
    }

