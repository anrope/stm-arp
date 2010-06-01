/*****************************************************************

 file: fir_filter.h
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

#ifndef ECE486_FIR_FILTER
#define ECE486_FIR_FILTER

/*
** Constants
*/
#define FIR_FBITS 14        /* Number of fractional bits used in input */
                            /* and output representations */

/*
** Parameter Structure Definitions
*/
typedef struct fir_struct {
    int    *b;		/* Array of coefficients b(n) */
    int    *hist;	        /* History array containing previous input samples */
    int	   M;		/* Number of coef's in the arrays */
    int	   index;	/* index of "oldest" sample in the history... which will
    			   be replaced by the next input sample. */
    } FIR_STRUCT;

/*
** Function Prototypes
*/
FIR_STRUCT *init_fir(int *fir_coefs, int n_coef);
int calc_fir( FIR_STRUCT *s, int x);
void destroy_fir(FIR_STRUCT *s);

#endif
