/*
arp/arpsample.{c,h} :
	Defines the interface that the user will use to handle data samples. There are
	routines for one sample at a time, as well as sample blocks. The interfaces
	provide samples sampled at 48 kHz, and expect to be able to output at 48 kHz.
*/

#ifndef __ARP_SAMPLE__
#define __ARP_SAMPLE__

#define ADCBUFLEN 200
#define DACBUFLEN 200

#define ADCWAIT 100
#define DACWAIT 100

// #define ADCTOQ14(sample) ((int16_t)(((int16_t)(sample)) ^ ((uint16_t)0x8000)))
// #define Q14TODAC(sample) ((uint16_t)(((int32_t)(sample)) ^ ((int32_t)0xffff8000)))

#define ADCTOQ14(sample) ((int32_t)(sample)-32768)
#define Q14TODAC(sample) (((sample)+32768))

extern volatile int newsample;

// typedef struct {
// 	volatile uint16_t * insamp;
// 	volatile uint16_t * outsamp;
// 	int nsamp;
// 	int cursamp;
// } sampleblock;

typedef struct {
	int nsamp;
	int * block;
} workingblock;

/*
	Simple accessor to return block size.
	Needed for creating a working buffer.
*/
int getblocksize(void);

/*
	getsample() returns a single sample to the user.

	This routine must be called often enough to prevent
	buffer overrun, with samples occuring at 48 kHz.

	In the event of an overrun, an error LED will come
	on (from flagerror()).

	Port C, pin 8 LED will be on while working on the
	lower half of the buffer.

	Port C, pin 5 LED will be on while working on the
	upper half of the buffer.

	When both pc8 and pc5 LEDs are off (high), the
	processor is idle.
*/
int32_t getsample(void);

/*
	putsample() takes a single sample from the user
	and places it in a buffer to be output by the
	DAC.
*/
void putsample(int32_t);

// sampleblock * prepblock(void);

/*
	getblock() is called by the user when the user is
	ready to process a new block of samples. The user
	provides a pointer to an already allocated buffer
	that will be filled with Q14 samples.
*/
void getblock(int * working);

/*
	putblock() is called by the user when they've finished
	processing a block and are ready for the DAC to output.

	Handles converting the samples to a format the DAC
	likes, and copies the samples to the DAC DMA's buffer.
*/
void putblock(int * working);
#endif