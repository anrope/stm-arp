#include <stm32f10x.h>
#include "arpinit.h"
#include "arpsample.h"
#include "arperr.h"
#include "biquad_filter.h"

BIQUAD_STRUCT * bs;

int filtout;

void main (void) {
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
	
	while (1)
	{		
// 		putsample(calc_biquad(bs,getsample()));
		
		putsample(getsample());
	}
	
	//never runs
	destroy_biquad(bs);
}





	
	


