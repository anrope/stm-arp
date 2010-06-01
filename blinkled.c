#include <stm32f10x.h>                              // file name depends on the device used.

int ledon = 0;
int count = 0;
int count2 = 0;
uint16_t mypin = GPIO_Pin_4;

void SysTick_Handler (void);

void main (void) {
	SysTick_Config(SystemCoreClock / 100000);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef portc;
	portc.GPIO_Pin = (GPIO_Pin_1 |
						GPIO_Pin_4 |
						GPIO_Pin_5 |
						GPIO_Pin_8 |
						GPIO_Pin_9);
	portc.GPIO_Speed = GPIO_Speed_10MHz;
	portc.GPIO_Mode = GPIO_Mode_Out_OD;
	
	GPIO_TypeDef *ledport;
	ledport = GPIOC;
	
// 	void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
	
	GPIO_Init(ledport, &portc);
	
// 	void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	
	GPIO_ResetBits(ledport, GPIO_Pin_1);
	
	GPIO_SetBits((GPIO_TypeDef *)GPIOC, GPIO_Pin_4 |
										GPIO_Pin_5 |
										GPIO_Pin_8 |
										GPIO_Pin_9);
	
	while (1)
	{	  

	  if (ledon)
	  {
		GPIO_ResetBits(ledport, mypin);
	  } else {
		GPIO_SetBits(ledport, mypin);
	  }
	}
}

void SysTick_Handler (void)
{
  ledon = !ledon;
  count2++;
  if (count2 >= 2)
  {
	count++;
	count2 = 0;
  }
  if (count >= 4) count = 0;
  GPIO_SetBits((GPIO_TypeDef *) GPIOC, mypin);
  switch (count) {
	case 0:
		mypin = GPIO_Pin_4;
		break;
	case 1:
		mypin = GPIO_Pin_5;
		break;
	case 2:
		mypin = GPIO_Pin_8;
		break;
	case 3:
		mypin = GPIO_Pin_9;
		break;
	default:
		mypin = GPIO_Pin_1;
  }
}
