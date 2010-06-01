/*****************************************************************

 file: biquad_filter.h
 Description:  Subroutines to implement a (real) IIR filter in real-time.
               The IIR filter is described as a cascade of 2nd-order "biquad"
               systems.  Any number of stages may be implemented.  Each
               stage is described by the difference equation

                 y(n) + a1*y(n-1) + a2*y(n-2) = b0*x(n) + b1*x(n-1) + b2*x(n-2)

               In addition, the cascaded system input is multiplied by 
               an arbitrary scale factor "scale", allowing the gain of the 
               cascaded system to be arbitrarily set.  (Note that a0=1
               is assumed for each stage).  
               
               Each second-order system is implemented using a Direct-Form II architecture.
               
               All filter inputs, outputs and coefficients are stored as "int" data types.
               Fixed point representations are assumed, with the least significant
               14 bits of the data word representing the fractional part of the stored value.
               
               Internal calculations utilize "int" data types with the least significant 
               28-bits used to represent the fractional part.  (For systems in which 
               a 32-bit word is used to represent an "int", all intermediate results
               must be less than +/-8 to avoid overflow.  Input samples and coefficients 
               must be scaled by the caller to avoid violating this limit.)
 

 Implementation:
   IIR filters are implemented using three functions:
     init_biquad() is called once, and is used to initialize the array
                  of filter coefficients for all the stages, 
                  allocate any required memory,
                  and perform any other required initialization.
     calc_biquad() is called multiple times -- once for every input sample.
                  It returns the output sample of the cascaded biquad systems.
     destroy_biquad() is called once at the end of the program, and is used
                  to de-allocate any memory.

  Function Prototypes and parameters:

        #include "biquad_filter.h"
        BIQUAD_STRUCT *init_biquad(
                int     sections,
                int     scale,
                int     a_coefs[][3],
                int     b_coefs[][3]);
                
         Inputs:                                                               
            The input values "scale", a_coefs[i][j], and b_coefs[i][j] are assumed 
            to be fixed-point representations, with the least-significant 14-bits 
            of the data word representing the fractional part.       
            
              sections        Number of Biquad sections                        
              scale           Filter input scale factor (see above)            
              a_coefs         "sections" by 3 array of "a" coefficients for    
                              the above difference equation.  a_coefs[i][j]    
                              should contain coefficients for the ith section  
                              stored as indicated below:                       

                              int   a_coefs[][3] = { {a0, a1, a2},  // First biquad  
                                                     {a0, a1, a2},  // second biquad 
                                                    . . .                      
                                                     {a0, a1, a2} }; // last biquad  

                              The "a0" coefficients are not used by this routine,
                              and are assumed to be a value of "1" (represented by
                              the integer 16384).

              b_coefs         "sections" by 3 array of "b" coefficients for    
                              the above difference equation.  b_coefs[i][j]    
                              should contain coefficients for the ith section  
                              stored as indicated below:                       

                              int   b_coefs[][3] = { {b0, b1, b2},    // First biquad  
                                                     {b0, b1, b2},    // second biquad 
                                                    . . .                      
                                                     {b0, b1, b2}  }; // last biquad   

         Returned:
              The function returns a pointer to a "BIQUAD_STRUCT" data type
              (which is defined in "biquad_filter.h")
              
      int calc_biquad( int x, BIQUAD_STRUCT *s);
            The input value "x", and the returned value are assumed 
            to be fixed-point representations, with the least-significant 14-bits 
            of the data word representing the fractional part.       

         Inputs:
              x       Input sample value
              s       pointer to BIQUAD_STRUCT, as provided by init_biquad()
         Returned:
              The function returns the filter output sample.
              
      void destroy_biquad(BIQUAD_STRUCT *s);   
         Inputs:
              s       pointer to BIQUAD_STRUCT, as provided by init_biquad()
         No value is returned.
*******************************************************************/ 
#ifndef ECE486_BIQUAD_FILTER
#define ECE486_BIQUAD_FILTER

/*
* Constants
*/
#define BIQUAD_FBITS 14     /* Number of fractional bits used in input */
                            /* and output representations */

/*
** Parameter Structure Definition
*/
typedef struct biquad_struct {
    int sections;	/* Number of 2nd-order sections */
    int scale;
    int (*a)[3];	/* Biquad denominator coefs for all sections */
    int (*b)[3];	/* Biquad Numerator coefs for all sections */
    int (*w)[2];	/* History array... two elements for each section */
    } BIQUAD_STRUCT;

/*
** Function Prototypes
*/
BIQUAD_STRUCT *init_biquad(
	int 	sections,
        int     scale,
	int	a_coefs[][3],
	int     b_coefs[][3]);
	
int calc_biquad( BIQUAD_STRUCT *s, int x );

void destroy_biquad(BIQUAD_STRUCT *s);
#endif
