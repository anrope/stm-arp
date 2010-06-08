#define FIR_FBITS 14

typedef struct {
	int *coefs;
	int *history;
	int ncoefs;
	int index;
	int skip;
	int skipct;
} firinfo;

firinfo * init_fir(int * coefs, int ncoefs, int skip, int skipoff);
int calc_fir(firinfo * f, int x);
void destroy_fir(firinfo * filter);
