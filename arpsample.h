#define ADCBUFLEN 100
#define DACBUFLEN 100

#define ADCWAIT 50
#define DACWAIT 50

extern volatile int newsample;

typedef struct {
	volatile int16_t * insamp;
	volatile uint16_t * outsamp;
	int nsamp;
	int cursamp;
} sampleblock;

typedef struct {
	int nsamp;
	int * block;
} workingblock;

int getblocksize(void);

int32_t getsample(void);
void putsample(int32_t);

sampleblock * prepblock(void);

void getblock(int * working);
void putblock(int * working);


