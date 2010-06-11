#define FIR_FBITS 14

typedef struct {
	int *coefs;
	int *history;
	int ncoefs;
	int index;
	int skip;
	int skipct;
} firinfo;

extern int16_t adcsamp;
extern int insamp;
extern int filtout;
extern uint32_t filtscaled;

firinfo * init_fir(int * coefs, int ncoefs, int skip, int skipoff);
// void calc_fir(firinfo * f, sampleblock * sb);
void calc_fir(firinfo * f, int * inbuf, int * outbuf, int nsamp);
void destroy_fir(firinfo * filter);
