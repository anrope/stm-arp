#define ERRORBUFLEN 100

void initerror(void);
void flagerror(int errorcode);

//error list
#define HSE_START_FAIL 1
#define SAMPLE_OVERRUN_LOWER 2
#define SAMPLE_OVERRUN_UPPER 3