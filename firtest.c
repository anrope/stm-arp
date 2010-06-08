#include <stm32f10x.h>
#include "arpinit.h"
#include "arpsample.h"
#include "arperr.h"
#include "arpfir.h"

firinfo * fs;

int filtout;

void main (void) {
	initialize();
			
	
// 	firinfo * init_fir(int * coefs, int ncoefs, int skip, int skipoff)
	fs = init_fir();
	
	while (1)
	{		
// 		putsample(calc_fir(fs,getsample()));
	}
	
	//never runs
	destroy_fir(fs);
}





	
	


