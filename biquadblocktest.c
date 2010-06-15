#include <stm32f10x.h>
#include <stdlib.h>
#include "arpinit.h"
#include "arpsample.h"
#include "arperr.h"
#include "biquadblock_filter.h"

BIQUAD_STRUCT * bs;

int filtout;

void main (void) {
	int nsamp;
	int * working;
	
	initialize();
			
	static int num_sections = 4;

	static int gain = 268;

	static int b_coefs[][3] = {
		{707, -572, 707},
		{4435, -7786, 4435},
		{3090, -5728, 3090},
		{581570, -1090477, 581570} };
	
	static int a_coefs[][3] = {
		{16384, -29651, 13535},
		{16384, -30188, 14679},
		{16384, -30668, 15667},
		{16384, -30979, 16200} };
		
	bs = init_biquad(num_sections, gain, a_coefs, b_coefs);
	
	nsamp = getblocksize();
	
	working = (int *)malloc(sizeof(int)*nsamp);
	
	while (1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_3);
		getblock(working);
		GPIO_ResetBits(GPIOC, GPIO_Pin_3);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		calc_biquad(bs, working, working, nsamp);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		putblock(working);
	}
	
	//never runs
	destroy_biquad(bs);
}





	
	


