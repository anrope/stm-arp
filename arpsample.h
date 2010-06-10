#define ADCBUFLEN 100
#define DACBUFLEN 100

#define ADCWAIT 50

extern volatile int newsample;

int16_t getsample(void);
void putsample(int32_t);

int16_t * getblock(void);
void putblock(int32_t *);