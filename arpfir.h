#define FIR_FBITS 14

typedef struct {
	int *coefs;
	int *history;
	int ncoefs;
	int index;
} firinfo;