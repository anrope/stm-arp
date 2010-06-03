#include <stm32f10x.h>
#include "biquad_filter.h"

#define DACCLK_CNT 1500
#define ADCCLK_CNT 1500

void initrcc(void);
void initgpio(void);
void initdac(void);
void initadc(void);
void initdacdma(void);
void initadcdma(void);
void inittim6(void);
void inittim3(void);
void cfgmco(void);
void cfgclock(void);
void initnvic(void);

volatile int16_t adcbuf;
volatile uint16_t dacbuf;

BIQUAD_STRUCT * bs;

int filtout;
uint32_t filtscaled;

volatile int newsample = 0;
int overrun = 0;

void main (void) {
	cfgclock();
	
	initrcc();
	initgpio();
	
	initdac();
	initdacdma();
	inittim6();
	
	initadc();
	initadcdma();
	initnvic();
	inittim3();
	
	cfgmco();
	
// 	static int num_sections = 4;
// 	
// 	static int gain = 8483;
// 	
// 	static int b_coefs[][3] = {
// 		{5298, 0, -5298},
// 		{42144, 0, 0},
// 		{5242, 0, 5242},
// 		{34347, -31186, 34347} };
// 		
// 	static int a_coefs[][3] = {
// 		{16384, 11556, 5898},
// 		{16384, 13002, 5898},
// 		{16384, -25269, 12688},
// 		{16384, -9620, 14787} };

// static int num_sections = 4;
// 
// static int gain = 21;
// 
// static int b_coefs[][3] = {
// 	{16384, -13242, 16384},
// 	{16384, -28762, 16384},
// 	{16384, -30369, 16384},
// 	{16384, -30721, 16384} };
// 	
// 	static int a_coefs[][3] = {
// 		{16384, -29651, 13535},
// 		{16384, -30188, 14679},
// 		{16384, -30668, 15667},
// 		{16384, -30979, 16200} };
		
static int num_sections = 4;

static int gain = 268;

static int b_coefs[][3] = {
	{707, -572, 707},
	{4435, -7786, 4435},
	{3090, -5728, 3090},
	{581570, -1090477, 581570} };
	
static int a_coefs[][3] = {
		{16384, -29651, 13535},
		{16384, -30188, 14679},
		{16384, -30668, 15667},
		{16384, -30979, 16200} };

	
// 	static int filtout;
// 	static uint32_t filtscaled;
		
	bs = init_biquad(num_sections, gain, a_coefs, b_coefs);
	
	while (1)
	{
		if (newsample)
		{
			//error: overrun
			overrun++;
		}
		
		while (!newsample);
		
		newsample = 0;
		
		adcbuf = adcbuf ^ 0x8000;
		
		filtout = calc_biquad(bs,adcbuf);

		filtscaled = filtout ^ 0xffff8000;
		
		dacbuf = filtscaled;
		
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	}
	
	//never runs
	destroy_biquad(bs);
}

void DMA1_Channel1_IRQHandler(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC1);
// 	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	newsample = 1;
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
}

void initnvic(void)
{
	NVIC_InitTypeDef nvic;
	
	nvic.NVIC_IRQChannel = DMA1_Channel1_IRQn;
// 	nvic.NVIC_IRQChannel = TIM3_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic);
}

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

void initgpio(void)
{
	//configure port a pin 4 (dac out 1) as an analog input
	//to avoid parasitic consumption (ref. stdperiph example)
	GPIO_InitTypeDef porta4;
	porta4.GPIO_Pin = GPIO_Pin_4;
	porta4.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(GPIOA, &porta4);
	
// 	//mco pin AF push-pull (PA8 pin 41)
// 	GPIO_InitTypeDef porta8;
// 	porta8.GPIO_Pin = GPIO_Pin_8;
// 	porta8.GPIO_Speed = GPIO_Speed_50MHz;
// 	porta8.GPIO_Mode = GPIO_Mode_AF_PP;
// 	
// 	GPIO_Init(GPIOA, &porta8);
	
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
	
	GPIO_SetBits(GPIOC, GPIO_Pin_4 |
						GPIO_Pin_5 |
						GPIO_Pin_8 |
						GPIO_Pin_9);
}

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

void initadc(void)
{
	//populate adc structure
	ADC_InitTypeDef adcinfo;
	adcinfo.ADC_Mode = ADC_Mode_Independent;
	adcinfo.ADC_ScanConvMode = DISABLE;
	adcinfo.ADC_ContinuousConvMode = DISABLE;
	adcinfo.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	// 	adcinfo.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adcinfo.ADC_DataAlign = ADC_DataAlign_Left;
	adcinfo.ADC_NbrOfChannel = 1;
	
	// 	void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);	
	// 	void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);
	// 	void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
	//apply settings to adc 1
	ADC_Init(ADC1, &adcinfo);
	
	// 	void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);
	
	// 	void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
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

void initadcdma(void)
{
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitTypeDef dmainfo;
	// 	dmainfo.DMA_PeripheralBaseAddr = DR_ADDRESS;
	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (0x4001244c);
	dmainfo.DMA_MemoryBaseAddr = (uint32_t) &adcbuf;
	dmainfo.DMA_DIR = DMA_DIR_PeripheralSRC;
	dmainfo.DMA_BufferSize = 1;
	dmainfo.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmainfo.DMA_MemoryInc = DMA_MemoryInc_Disable;
	dmainfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmainfo.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dmainfo.DMA_Mode = DMA_Mode_Circular;
	dmainfo.DMA_Priority = DMA_Priority_High;
	dmainfo.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel1, &dmainfo);
	
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void initdacdma(void)
{
	DMA_DeInit(DMA2_Channel3);
	
	DMA_InitTypeDef dmainfo;
	// 	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (DAC_BASE + DHR12R1_Offset);
	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (0x4000740C);
	dmainfo.DMA_MemoryBaseAddr = (uint32_t) &dacbuf;
	dmainfo.DMA_DIR = DMA_DIR_PeripheralDST;
	dmainfo.DMA_BufferSize = 1;
	dmainfo.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmainfo.DMA_MemoryInc = DMA_MemoryInc_Disable;
	dmainfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmainfo.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dmainfo.DMA_Mode = DMA_Mode_Circular;
	dmainfo.DMA_Priority = DMA_Priority_High;
	dmainfo.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA2_Channel3, &dmainfo);
	
	DMA_Cmd(DMA2_Channel3, ENABLE);
}

void inittim6(void)
{
	// 	TIM_SetAutoreload(TIM6, 0xf);
	// 	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	// 	TIM_Cmd(TIM6, ENABLE);
	
	TIM_TimeBaseInitTypeDef timinfo;
	timinfo.TIM_Prescaler = TIM_CKD_DIV1;
	timinfo.TIM_CounterMode = TIM_CounterMode_Down;
	timinfo.TIM_Period = DACCLK_CNT;
	timinfo.TIM_ClockDivision = 0;
	//repetition counter isn't used for tim6
	timinfo.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM6, &timinfo);
	
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM6, ENABLE);
}

void inittim3(void)
{
	TIM_TimeBaseInitTypeDef timinfo;
	timinfo.TIM_Prescaler = TIM_CKD_DIV1;
	timinfo.TIM_CounterMode = TIM_CounterMode_Down;
	timinfo.TIM_Period = ADCCLK_CNT;
	timinfo.TIM_ClockDivision = 0;
	//repetition counter isn't used for tim3
	timinfo.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3, &timinfo);
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	
// 	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM3, ENABLE);
}

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
		//pll output is hse x9
		//hse = 8mhz, sysclk = 72mhz
		RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div1);
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
		
		//prediv2 brings hse to pll2 and pll3
		RCC_PREDIV2Config(RCC_PREDIV2_Div1);
		
		//pll2 and pll3 only use for i2s?
		RCC_PLL2Config(RCC_PLL2Mul_8);
		RCC_PLL3Config(RCC_PLL3Mul_8);
		
		//configure adc prescaler to set
		//adcclk = 12mhz (spec'd fmax = 14mhz)
		//12mhz (/6) is closest we can get
		//because hclk2 = 72mhz
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);
		
		//enable pll
		RCC_PLLCmd(ENABLE);
		
		//wait for pll to become stable
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {}
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//wait for pll to be used for sysclk
		while (RCC_GetSYSCLKSource() != 0x08) {}
		
		
		//even though we're running off
		//hse and pll now, we can't disable
		//hsi.
		//this makes it impossible to write to
		//flash (not totally sure why).
	}
}
	
	


