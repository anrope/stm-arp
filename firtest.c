#include <stm32f10x.h>
#include "arpinit.h"
#include "arpsample.h"
#include "arperr.h"
#include "arpfir.h"

firinfo * fs;

int filtout;

static int num_coef = 1;
// static int num_coef = 5;

static int b_coefs[] = {
	13,	46,	101,	175,	241,
	271,	232,	119,	-45,	-197,
	-267,	-206,	-18,	222,	394,
	384,	146,	-254,	-639,	-780,
	-492,	276,	1394,	2577,	3477,
	3812,	3477,	2577,	1394,	276,
	-492,	-780,	-639,	-254,	146,
	384,	394,	222,	-18,	-206,
	-267,	-197,	-45,	119,	232,
	271,	241,	175,	101,	46,
	13 };

// static int b_coefs[] = {
// 	16384, 16384, 16384, 16384, 16384};

void main (void) {
	initialize();
			
	
// 	firinfo * init_fir(int * coefs, int ncoefs, int skip, int skipoff)
	fs = init_fir(b_coefs, num_coef, 0, 0);
	
	while (1)
	{		
// 		filtout = calc_fir(fs,getsample());
// 		putsample(filtout);
		putsample(getsample());
	}
	
	//never runs
	destroy_fir(fs);
}





	
	


