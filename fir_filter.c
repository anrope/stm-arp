/*****************************************************************

 file: fir_filter.c
 Description:  Subroutines to implement a FIR Filter in real-time.
                 Multiple subroutine calls are used to calculate the output
               of an FIR filter
                     y(n) = x(n) * h(n)
                 where x(n) is the nth input sample, and the values of h(n) 
                 obtained from an array of fixed values, and "*" denotes 
               discrete-time convolution.

 Implementation:
   Filters are implemented using three function calls:
     init_fir()   is called once, and is used to initialize the array
                  of filter coefficients h(n), allocate any required memory,
                    and perform any other required initialization.
     calc_fir()   is called multiple times -- once for every input sample.
                  It returns the filter output sample y(n).
     destroy_fir() is called once at the end of the program, and is used
                  to deallocate any memory which was allocated by init_fir().

  Function Prototypes and parameters:

        #include "fir_filter.h"
        FIR_STRUCT *init_fir(int *filter_coefs, int n_coef);

           Inputs:
                filter_coefs    pointer to the array of values for h(n)
                                filter_coefs[i] should contain the impulse
                                response of the filter at time "i", for 
                                i=0, 1, 2, ..., n_coef-1
                                The filter_coefs array is NOT modified by
                                init_fir(), calc_fir(), or destroy_fir().
                                Coefficients are assumed to be stored using a  
                                fixed point representation, with the least-significant
                                14 bits of the data word representing the 
                                fractional part.

                n_coef          Number of samples in the impulse response array

           Returned:
                The function returns a pointer to a "FIR_STRUCT" data type
                (which is defined in "fir_filter.h")
                
        int calc_fir( FIR_STRUCT *s, int x );

           Inputs:
                s       pointer to FIR_STRUCT, as provided by init_fir().  The
                      contents of the structure s are modified by each called
                      to calc_fir()
                x       Input sample value:
                      Sample values are assumed to be stored using a  
                      fixed point representation, with the least-significant
                      14 bits of the data word representing the 
                      fractional part.

           Returned:
                The function returns the FIR filter output sample y(n).

                      i=n_coef-1
                y(n) =    sum     ( h[i] x(n-i) )
                          i=0

                where x(n) is the value of "x" for the current call, x(n-1)
                is the value of "x" from the previous call, x(n-2) is the 
                value of "x" from call before that, and so on.  
                The return value y(n) is represented as a fixed point integer,
                with the least-significant 14 bits of the data word representing
                its fractional part.
                
        void destroy_fir(FIR_STRUCT *s);   
           Inputs:
                s       pointer to FIR_STRUCT, as provided by init_fir()
           No value is returned.
 
   NOTE:  Internal FIR Filter calculations are performed using 32-bit signed 
        integer representations with the least-significant 28 bits used to
        represent the fractional part.  As a result, input samples and filter 
        coefficients must be scaled by the caller so that the filter output
        remains within the bounds -8 < y(n) < 8.  Failure to do so will 
        result in numerical overflow and severe distortion of the output 
        signal.  This routine does NOT check for overflow.
       
*******************************************************************/ 

#include <stdlib.h>
#include "fir_filter.h"

FIR_STRUCT *init_fir(int *fir_coefs, int n_coef)
    {
    FIR_STRUCT *result;
    int i;
    
    /*
    ** Allocate space for the filter structure
    */
    result = (FIR_STRUCT *)malloc( sizeof(FIR_STRUCT) );

    result->M = n_coef;	/* Number of filter coef's */
    
    /*
    ** FIR coef's are copied here in the REVERSED order to make the 
    ** code in calc_fir() simpler...  
    */
    result->b = (int *)malloc( n_coef * sizeof(int));
    for (i=0; i<n_coef; i++)
        result->b[i] = fir_coefs[n_coef-1-i];
    
    /*
    ** The history buffer will save the input samples from one call to the next.
    ** It's a "circular buffer" with the oldest sample index provided by 
    ** result.index. So on the nth call we have
    **
    **		hist[0]:		x(n-index+1)
    **		...
    **		hist[index-2]:		x(n-1)		(2nd newest sample)
    **        	hist[index-1]:		x(n)		(newest sample)
    **		hist[index]:		x(n-(M-1))	(oldest sample)
    **		hist[index+1]:		x(n-(M-2))	(2nd oldest sample)
    **		...
    **		hist[M-1]		x(n-index)
    */
    result->hist = (int *)malloc( n_coef * sizeof(int));
    for (i=0; i<n_coef; i++) result->hist[i] = 0.0;
    result->index = 0;
    
    return(result);
    }

int calc_fir( FIR_STRUCT *s, int x )
    {
    int y;
    int *fir_ptr;
    int i;
    
    /*
    ** Update the history buffer by replacing the oldest sample in the buffer
    ** by the new input sample.  Make sure s->index points to the OLDEST
    ** input sample, as described above.
    */
    s->hist[ s->index ] = x;
    s->index += 1;		
    if (s->index == s->M) s->index = 0;
    
    /*
    ** Now evaluate the convolution.  Note that the FIR coef's have been
    ** reversed in the s->b array
    */
    y = 0;
    fir_ptr = s->b;	/* pointer to "current" filter coef */
    for (i=s->index; i<s->M; i++)	/* 1st loop for the oldest samples... */
        y += *(fir_ptr++) * s->hist[i];
	
    for (i=0; i<s->index; i++)		/* 2nd loop for the most recent input samples */
        y += *(fir_ptr++) * s->hist[i];
    
    return(y>>FIR_FBITS);
    }

void destroy_fir(FIR_STRUCT *s)
    {
    free(s->b);
    free(s->hist);
    free(s);
    }


