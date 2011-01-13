/*
arp/arpinit.{c,h} :
	Contains all of the subroutines to configure various peripherals of the stm32,
	such as RCC, GPIO PORT{A,B,C,...}, NVIC, USB, USART, SPI, etc.
	More information regarding configuration of these peripherals is found in the
	CMSIS and STM32F10x_StdPeriph_Driver documentation.
*/

#ifndef __ARP_INIT__
#define __ARP_INIT__

#define DACCLK_CNT 1500
#define ADCCLK_CNT 1500

#define FS_48K	1500
#define FS_24K	3000
#define FS_8K	9000

#define MONO_IN 	0
#define STEREO_IN	1

#define MONO_OUT	0
#define STEREO_OUT	1

extern volatile uint16_t adcbuf;
extern volatile uint32_t dacbuf;

/*
	initnvic() configures the Nested Vector Interrupt Controller
	(NVIC). All interrupts that are going to be used must be 
	enabled here.
*/
void initnvic(void);

/*
	initrcc() configures the Reset and Clock Control (RCC)
	module to enable the clocks to each peripheral.

	By only clocking the necessary peripherals, less power
	is used.
*/	
void initrcc(void);

/*
	initgpio() configures the gpio ports, controlling the
	electrical properties of each pin (input, output, 
	push pull, open drain, etc. See STM32 Reference Manual)

	Usually pins being used by peripherals (such as USB,
	SPI, etc.) need to be configured in a particular way
	in the gpio registers (described in the STM32
	Reference Manual section for each peripheral).
*/
void initgpio(void);

/*
	initdac() configures the digital-to-analog converter
	(DAC). Timer T6 triggers the DAC, a DMA request is made
	for new data, and then the conversion is performed.
*/
void initdac(void);
void initdac2(void);

/*
	initadc() configures the analog-to-digital converter
	(ADC). When timer T3 triggers the ADC, a conversion is
	performed. The ADC hands off the new sample to the DMA,
	which delivers the sample to a buffer.

	initadc() also runs the built-in ADC calibration.
*/
void initadc(void);

/*
	initadcdma() configures the DMA that is used with
	the ADC. This DMA is triggered by an ADC request.
	The DMA takes a sample from the ADC, and brings it
	to a buffer.

	This routine also enables the half transfer and
	transfer complete interrupts for the DMA.

	The STM32 Reference Manual and StdPeriph documentation
	have more information about the actual DMA settings.
*/
void initadcdma(void);

/*
	initdacdma() configures the DMA that is used with
	the DAC. This DMA is triggered by a DAC request.
	The DMA grabs a sample from a buffer, and delivers
	it to the DAC.

	The DAC has multiple input registers, each for a
	different bit alignment. It's important to make sure
	the correct DMA_PeripheralBaseAddr is chosen to
	use the desired bit alignment.

	The STM32 Reference Manual and StdPeriph documentation
	have more information about the actual DMA settings.
*/
void initdacdma(int chanin);

/*
	inittim6() configures timer 6 to roll over at a
	frequency of 48kHz, and to trigger the DAC every
	time it rolls over.
*/
void inittim6(int daccount);

/*
	inittim3() configures timer 3 to roll over at a
	frequency of 48kHz, and to trigger the ADC every
	time it rolls over.
*/
void inittim3(int adccount);

/*
	cfgmco() configures port A, pin 8 to output
	PLLCLK/2. This is useful for making sure the
	external oscillator is behaving correctly.
*/
void cfgmco(void);

/*
	cfgclock() configures the chip to use the
	external (8 MHz) oscillator (HSE = High Speed
	External oscillator), and to set PLLs
	to clock SYSCLK at 72 MHz.

	Before changing anything here, make sure you
	stare at the clock tree in the STM32 Reference
	Manual for a while.

	With HSE driving the chip, HSI (High Speed
	Internal oscillator 8MHz) needs to remain enabled
	in order to be able to write to flash.
*/
void cfgclock(void);

/*
	initialize() is just a wrapper function to
	be called in user programs. It calls the init
	functions necessary to run filters.
*/
void initialize(int fs, int chanin, int chanout);
#endif