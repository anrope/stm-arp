#include <stm32f10x.h>

extern int cursamp;
extern int lowerrdy;

volatile int newsample = 0;

void DMA1_Channel1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_HT1))
	{
		DMA_ClearITPendingBit(DMA1_IT_HT1);
		//lower half good
		lowerrdy = 1;
	} else if (DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		//upper half good
		lowerrdy = 0;
	}
	
	DMA_ClearITPendingBit(DMA1_IT_TC1);
// 	newsample = 1;
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);
}