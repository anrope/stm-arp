#define ADCBUFLEN 200
#define DACBUFLEN 200

#define ADCWAIT 100
#define DACWAIT 100

// #define ADCTOQ14(sample) ((int16_t)(((int16_t)(sample)) ^ ((uint16_t)0x8000)))
// #define Q14TODAC(sample) ((uint16_t)(((int32_t)(sample)) ^ ((int32_t)0xffff8000)))

#define ADCTOQ14(sample) ((int32_t)(sample)-32768)
#define Q14TODAC(sample) (((sample)+32768))

extern volatile int newsample;

typedef struct {
	volatile uint16_t * insamp;
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


