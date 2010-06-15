#include <stm32f10x.h>
#include <stdlib.h>
#include "arpinit.h"
#include "arpsample.h"
#include "arperr.h"
#include "arpfirblock.h"

firinfo * fs;

int filtout;

static int num_coef = 51;
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
	int nsamp;
	int * working;

	initialize();

	fs = init_fir(b_coefs, num_coef, 0, 0);

	nsamp = getblocksize();

	working = (int *)malloc(sizeof(int)*nsamp);
	
	while (1)
	{
		getblock(working);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
		calc_fir(fs, working, working, nsamp);
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		putblock(working);
	}
	
	//never runs
	destroy_fir(fs);
}





	
	


