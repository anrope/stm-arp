/*
arp/arpinit.{c,h} :
	Contains all of the subroutines to configure various peripherals of the stm32,
	such as RCC, GPIO PORT{A,B,C,...}, NVIC, USB, USART, SPI, etc.
	More information regarding configuration of these peripherals is found in the
	CMSIS and STM32F10x_StdPeriph_Driver documentation.
*/

#include <stm32f10x.h>
#include "arpinit.h"
#include "arpsample.h"
#include "arperr.h"

/*
	initnvic() configures the Nested Vector Interrupt Controller
	(NVIC). All interrupts that are going to be used must be 
	enabled here.
*/
void initnvic(void)
{
	NVIC_InitTypeDef nvic;
	
	nvic.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic);
	
	nvic.NVIC_IRQChannel = DMA2_Channel3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = DISABLE;
	
	NVIC_Init(&nvic);
}

/*
	initrcc() configures the Reset and Clock Control (RCC)
	module to enable the clocks to each peripheral.

	By only clocking the necessary peripherals, less power
	is used.
*/	
void initrcc(void)
{
	//enable peripheral clocks
	
	//apb2 max clock 72mhz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//apb1 max clock 36mhz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//ahb max clock 72mhz
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
}

/*
	initgpio() configures the gpio ports, controlling the
	electrical properties of each pin (input, output, 
	push pull, open drain, etc. See STM32 Reference Manual)

	Usually pins being used by peripherals (such as USB,
	SPI, etc.) need to be configured in a particular way
	in the gpio registers (described in the STM32
	Reference Manual section for each peripheral).
*/
void initgpio(void)
{
	//configure port a pins 4, 5 (dac out 1, dac out 2) as analog inputs
	//to avoid parasitic consumption (ref. stdperiph example)
	GPIO_InitTypeDef porta4;
	porta4.GPIO_Pin = (GPIO_Pin_4 |
	GPIO_Pin_5);
	porta4.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(GPIOA, &porta4);
	
	//configure port c pin 0 (adc in 10) as an analog input
	//to avoid parasitic consumption (ref. stdperiph example)
	GPIO_InitTypeDef portc4;
	portc4.GPIO_Pin = GPIO_Pin_0;
	portc4.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(GPIOC, &portc4);
	
	//populate structure to define pins connected to LEDs
	//to be 10MHz open-drain outputs
	GPIO_InitTypeDef portc;
	portc.GPIO_Pin = (GPIO_Pin_1 |
	GPIO_Pin_4 |
	GPIO_Pin_5 |
	GPIO_Pin_8 |
	GPIO_Pin_9);
	portc.GPIO_Speed = GPIO_Speed_10MHz;
	portc.GPIO_Mode = GPIO_Mode_Out_OD;
	
	//Apply the settings to port c
	GPIO_Init(GPIOC, &portc);

	portc.GPIO_Pin = (GPIO_Pin_3 |
	GPIO_Pin_6);
	portc.GPIO_Speed = GPIO_Speed_10MHz;
	portc.GPIO_Mode = GPIO_Mode_Out_PP;

	GPIO_Init(GPIOC, &portc);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_4 |
	GPIO_Pin_5 |
	GPIO_Pin_8 |
	GPIO_Pin_9);
}

/*
	initdac() configures the digital-to-analog converter
	(DAC). Timer T6 triggers the DAC, a DMA request is made
	for new data, and then the conversion is performed.
*/
void initdac(void)
{
	//populate dac structure
	DAC_InitTypeDef dacinfo;
	dacinfo.DAC_Trigger = DAC_Trigger_T6_TRGO;
	dacinfo.DAC_WaveGeneration = DAC_WaveGeneration_None;
	dacinfo.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	dacinfo.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	
	//apply settings to dac channel 1
	DAC_Init(DAC_Channel_1, &dacinfo);
	
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	
	//enable dac
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void initdac2(void)
{
	//populate dac structure
	DAC_InitTypeDef dacinfo;
	dacinfo.DAC_Trigger = DAC_Trigger_T6_TRGO;
	dacinfo.DAC_WaveGeneration = DAC_WaveGeneration_None;
	dacinfo.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	dacinfo.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	
	//apply settings to dac channel 2
	DAC_Init(DAC_Channel_2, &dacinfo);
	
	//enable dac
	DAC_Cmd(DAC_Channel_2, ENABLE);
}

/*
	initadc() configures the analog-to-digital converter
	(ADC). When timer T3 triggers the ADC, a conversion is
	performed. The ADC hands off the new sample to the DMA,
	which delivers the sample to a buffer.

	initadc() also runs the built-in ADC calibration.
*/
void initadc(void)
{
	//populate adc structure
	ADC_InitTypeDef adcinfo;
	adcinfo.ADC_Mode = ADC_Mode_Independent;
	adcinfo.ADC_ScanConvMode = DISABLE;
	adcinfo.ADC_ContinuousConvMode = DISABLE;
	adcinfo.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	adcinfo.ADC_DataAlign = ADC_DataAlign_Left;
	adcinfo.ADC_NbrOfChannel = 1;
	
	//apply settings to adc 1
	ADC_Init(ADC1, &adcinfo);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);
	
	ADC_Cmd(ADC1, ENABLE);
	
	//Do an ADC reset calibration. block until finished.
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	//Do an ADC start calibration. block until finished.
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

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
void initadcdma(void)
{
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitTypeDef dmainfo;
	// 	dmainfo.DMA_PeripheralBaseAddr = DR_ADDRESS;
	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (0x4001244c);
	dmainfo.DMA_MemoryBaseAddr = (uint32_t) &adcbuf;
	dmainfo.DMA_DIR = DMA_DIR_PeripheralSRC;
	dmainfo.DMA_BufferSize = ADCBUFLEN;
	dmainfo.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmainfo.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmainfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmainfo.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dmainfo.DMA_Mode = DMA_Mode_Circular;
	dmainfo.DMA_Priority = DMA_Priority_High;
	dmainfo.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel1, &dmainfo);
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_HT, ENABLE);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

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
void initdacdma(int chanin)
{
	DMA_DeInit(DMA2_Channel3);
	
	DMA_InitTypeDef dmainfo;
	if (chanin == MONO_IN)
	{
	// 	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (DAC_BASE + DHR12L1_Offset);
		dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (0x4000740C);
	} else if (chanin == STEREO_IN)
	{
		dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (0x40007424);

	}
	dmainfo.DMA_MemoryBaseAddr = (uint32_t) &dacbuf;
	dmainfo.DMA_DIR = DMA_DIR_PeripheralDST;
	dmainfo.DMA_BufferSize = DACBUFLEN;
	dmainfo.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmainfo.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmainfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	dmainfo.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	dmainfo.DMA_Mode = DMA_Mode_Circular;
	dmainfo.DMA_Priority = DMA_Priority_High;
	dmainfo.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA2_Channel3, &dmainfo);
	
	//these interrupts aren't actually being used.
	//DMA2_Channel3 interrupts are disabled in the NVIC.
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA2_Channel3, DMA_IT_HT, ENABLE);
	
	DMA_Cmd(DMA2_Channel3, ENABLE);
}

/*
	inittim6() configures timer 6 to roll over at a
	frequency of 48kHz, and to trigger the DAC every
	time it rolls over.
*/
void inittim6(int daccount)
{	
	TIM_TimeBaseInitTypeDef timinfo;
	timinfo.TIM_Prescaler = TIM_CKD_DIV1;
	timinfo.TIM_CounterMode = TIM_CounterMode_Down;
// 	timinfo.TIM_Period = DACCLK_CNT;
	timinfo.TIM_Period = daccount;
	timinfo.TIM_ClockDivision = 0;
	//repetition counter isn't used for tim6
	timinfo.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM6, &timinfo);
	
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM6, ENABLE);
}

/*
	inittim3() configures timer 3 to roll over at a
	frequency of 48kHz, and to trigger the ADC every
	time it rolls over.
*/
void inittim3(int adccount)
{
	TIM_TimeBaseInitTypeDef timinfo;
	timinfo.TIM_Prescaler = TIM_CKD_DIV1;
	timinfo.TIM_CounterMode = TIM_CounterMode_Down;
// 	timinfo.TIM_Period = ADCCLK_CNT;
	timinfo.TIM_Period = adccount;
	timinfo.TIM_ClockDivision = 0;
	//repetition counter isn't used for tim3
	timinfo.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3, &timinfo);
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM3, ENABLE);
}

/*
	cfgmco() configures port A, pin 8 to output
	PLLCLK/2. This is useful for making sure the
	external oscillator is behaving correctly.
*/
void cfgmco(void)
{
	//mco pin AF push-pull (PA8 pin 41)
	GPIO_InitTypeDef porta8;
	porta8.GPIO_Pin = GPIO_Pin_8;
	porta8.GPIO_Speed = GPIO_Speed_50MHz;
	porta8.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_Init(GPIOA, &porta8);
	
	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
}

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
void cfgclock(void)
{
	RCC_HSICmd(ENABLE);
	
	//wait for hsi to become stable
	while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != SET) {}
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	
	//wait for hsi to be used for sysclk
	while (RCC_GetSYSCLKSource() != 0x00) {}
	
	//reset rcc to default state
	RCC_DeInit();
	
	//enable hse
	RCC_HSEConfig(RCC_HSE_ON);
	
	//wait for hse to become stable
	if (RCC_WaitForHSEStartUp() == ERROR)
	{
		//hse didn't start up :(
		flagerror(HSE_START_FAIL);
	} else {
		//hse started up
		
		//configure flash memory for sysclk = 72mhz
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);
		
		//hclk = ahb clk = sysclk
		//pclk1 = apb1 clk = sysclk/2
		//pclk2 = apb2 clk = sysclk
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		
		//pll will drive sysclk
		//pll2 and pll3 are going
		//to stay disabled
		RCC_PLLCmd(DISABLE);
		RCC_PLL2Cmd(DISABLE);
		RCC_PLL3Cmd(DISABLE);
		
		//use hse as prediv1 source
		//hse drives pll
		//pll output is pll2 x9
		//hse = 25mhz, pll2 = 40mhz, sysclk = 72mhz
		RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
		
		//prediv2 brings hse to pll2 and pll3
		//predive = 5 for 25mhz hse
		RCC_PREDIV2Config(RCC_PREDIV2_Div5);	

		//pll3 only used for i2s?
		//pll2 used to get 72mhz from 25mhz hse
		RCC_PLL2Config(RCC_PLL2Mul_8);
		RCC_PLL3Config(RCC_PLL3Mul_8);
		
		//configure adc prescaler to set
		//adcclk = 12mhz (spec'd fmax = 14mhz)
		//12mhz (divider = 6) is closest we can get
		//because hclk2 = 72mhz
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);
		
		//enable pll
		RCC_PLLCmd(ENABLE);
		RCC_PLL2Cmd(ENABLE);

		//wait for pll2 to become stable
		while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) != SET) {}
		
		//wait for pll to become stable
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {}
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//wait for pll to be used for sysclk
		while (RCC_GetSYSCLKSource() != 0x08) {}
		
		
		//even though we're running off
		//hse and pll now, we can't disable
		//hsi.
		//this makes it impossible to write to
		//flash.
	}
}

/*
	initialize() is just a wrapper function to
	be called in user programs. It calls the init
	functions necessary to run filters.
*/
void initialize(int fs, int chanin, int chanout)
{
	initerror();
	
	cfgclock();
	
	initrcc();
	initgpio();
	
	if (chanin == MONO_IN)
	{
		initdac();
	} else if (chanin = STEREO_IN)
	{
		initdac();
		initdac2();
	} else
	{
		flagerror(DAC_CONFIG_ERROR);
	}
	initdacdma(chanin);
	inittim6(fs);
	
	if (chanout == MONO_OUT)
	{
		initadc();
	} else if (chanout == STEREO_OUT)
	{
// 		initadcstereo();
		initadc();
	} else
	{
		flagerror(ADC_CONFIG_ERROR);
	}
	initadcdma();
	initnvic();
	inittim3(fs);
	
	cfgmco();
}