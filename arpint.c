/*
arp/arpint.{c,h} :
	Defines interrupt handler routines for the interrupts being used.
	The actual symbols for interrupt handlers are created in the startup script
	startup_stm32f10x_cl.s (in the CMSIS lib), because they must appear in
	predefined positions in the interrupt vector.
*/

#include <stm32f10x.h>
#include "arpint.h"
#include "arperr.h"

/*
	This handles the interrupt generated when the
	ADC buffer gets half full or full.

	These interrupts are used to make sure we don't
	underrun or overrun our buffers.
*/
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
	} else
	{
		flagerror(DEBUG_ERROR);
	}
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	GPIO_ResetBits(GPIOC, GPIO_Pin_8);
}