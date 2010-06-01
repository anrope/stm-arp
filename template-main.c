#include <stm32f10x.h>                              // file name depends on the device used.

void SysTick_Handler (void)  {                   // SysTick Interrupt Handler
  ;
}

void TIM1_UP_IRQHandler (void)  {                // Timer Interrupt Handler
  ;
}

void timer1_init(int frequency) {
                                                 // set up Timer (device specific)
  NVIC_SetPriority (TIM1_UP_IRQn, 1);            // Set Timer priority
  NVIC_EnableIRQ (TIM1_UP_IRQn);                 // Enable Timer Interrupt
}

void main (void) {
  SystemInit ();

	int SystemFrequency = 72000000;

  if (SysTick_Config (SystemFrequency / 1000)) { // Setup SysTick Timer for 1 msec interrupts
    //:                                            // Handle Error
    //:
    while (1);
  }

  timer1_init ();                                // device specific timer

  while (1);
}
