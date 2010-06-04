#define DACCLK_CNT 1500
#define ADCCLK_CNT 1500

extern volatile int16_t adcbuf;
extern volatile uint16_t dacbuf;

void initnvic(void);
void initrcc(void);
void initgpio(void);
void initdac(void);
void initadc(void);
void initadcdma(void);
void initdacdma(void);
void inittim6(void);
void inittim3(void);

void cfgmco(void);
void cfgclock(void);

void initialize(void);