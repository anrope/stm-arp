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
	
void calc_biquad( BIQUAD_STRUCT *s, int * inbuf, int * outbuf, int nsamp);
	
void destroy_biquad(BIQUAD_STRUCT *s);
#endif
