#include <stm32f10x.h>
#include <stdlib.h>
#include "arplibs.h"

void main (void) {
	int nsamp;
	int * working;
	
	initialize();
	
	nsamp = getblocksize();
	
	working = (int *)malloc(sizeof(int)*nsamp);
	
	while (1)
	{
		getblock(working);
		//signal processing code goes to work on the working buffer
		putblock(working);
	}
}





	
	


