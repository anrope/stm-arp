#include <stm32f10x.h>
#include "arpint.h"

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
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);
}