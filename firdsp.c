#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dsp.h"
#include "lab2_fir_h.h"
#include "fir_filter.h"

/*
** These default values can be overridden using command-line arguments.
**     "dsp486"    		uses the values given here....
**     "dsp486 -dsp=50000"	resets cmax to 50000...
**     "dsp486 -dsp=50000,3"	resets cmax to 50000 and attn to 3
*/
static int cmax=100; 	/* Number of input buffers processed until termination */
static int attn=1;	/* Attenuation factor applied to the right channel */

FIR_PARMS * fp1;

/* dsp_construct()
** Prepare for processing the audio data...
**   - Interpret command line arguments
**   - Allocate any required memory
**   - Initiallize filters and filter coefficients
**
**   Inputs:
**	count	Number of integers specified in the "-dsp" flag of the dsp486 command
**	vals	Array of integers providing the values specified in the -dsp flag
**		of the dsp486 command
**	c_buf_size Integer giving Maximum size (in bytes) for any input sample buffer
**
**  For example, using "dsp486 -dsp=50000,3" should provide 
**  count=2, vals[0]=50000, and vals[1]=3
**		
*/
void dsp_construct( int count, int * vals, size_t c_buf_size) {
    /*
    ** Interpret the command-line arguments...
    **
    **  Use the following only as an example.  In this demo program, the first 
    **  integer specified gives the number of input buffers to process
    **  (default = 100 buffers).  If provided, the second integer gives
    **  an attenuation factor which will be applied in copying the 
    **  samples from the input buffer to the output buffer.  In this 
    **  example, we just save these values to some static variables, so that 
    **  they'll be available later, when dsp_process() is called.
    */
    if (count>=1) {
	cmax = vals[0];
    }
    
    if (count>=2) {
	attn = vals[1];
    }



    /*
    ** Allocate memory...  Initialize filters... etc.
    ** (Not needed in this demo program)
    */

	fp1 = init_fir(dspb,dspnb);

    return;
}

/*  dsp_process()
**  Process an audio sample buffer
**
**  dsp_process is called multiple times... once for every new buffer of 
**  input sample data.  
**
**  Inputs:
**	ibuf	Array giving the input samples as 16-bit signed integers.
**		Inputs samples are interleaved, so that even-indexed samples
**		represent data from the left audio channel, and odd-indexed
**		values represent samples from the right channel.
**
**	isize	The total (left + right) number of samples in the ibuf array
**
**	obuf	Pointer to an array to which output samples may be written
**
**  Outputs:
**	obuf	On return, obuf[i] should contain the array of output samples 
**		to be written to the sound card DAC.  The data should be 
**		interleaved, so that even-indexed samples
**		represent data for the left audio channel, and odd-indexed
**		values represent samples for the right channel.
**
**	*osize	should contain the number of output samples
**		to be written to the DAC (usually = "isize").
**
**  Returned Value:  Normally, the number of output samples for the DAC
**  	is returned.  A return value of "-1" indicates that the dsp486 
** 	process should terminate.
**		
*/
int dsp_process(short * ibuf, size_t isize, short *obuf, size_t *osize) {
    int i;
    static int count=0;
    double insamp, outsamp;

    /*
    ** For the demo, the left channel is replaced by a full-wave 
    ** rectified version...
    */
    for (i=0;i<isize;i+=2) {	// Left ch
	insamp = ibuf[i];
	outsamp = fabs(insamp);
	obuf[i]=outsamp;
    }

    /*
    ** Here, the right channel is just attenuated, and copied to the output
    ** array
    */
    for (i=1; i<isize; i+=2) {	// right ch
	//obuf[i] =ibuf[i]/attn;
	obuf[i] = calc_fir(fp1,ibuf[i]);
    }

    // Set the number of output samples.
    *osize=isize;

    // Quit after cmax function calls...
    count++;
    if (count>cmax) {
	return -1;
    }

    // Return number of samples in obuf.
    return *osize;
}

/* dsp_destroy()
** Cleanup after finishing processing...
**
**  dsp_destroy is called only once, as the dsp486 process is exiting.
**  It allows you to do any house-keeping to leave the machine in a clean state.
**  As a good practice, you should free up any memory which was allocated in 
**  any of the above routines.
**
** (No inputs or outputs).
*/
void dsp_destroy(void) {
	
	destroy_fir(fp1);

	return;
}
