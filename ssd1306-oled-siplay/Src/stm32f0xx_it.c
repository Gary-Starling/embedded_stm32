
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_it.h"
#include "ssd1306.h"

#ifdef DEBUG

#endif




extern uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];

            

void NMI_Handler(void)
{
  
}


void HardFault_Handler(void)
{
  
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
  }
}


void SVC_Handler(void)
{
  
}


void PendSV_Handler(void)
{
  
}


void SysTick_Handler(void)
{
  HAL_IncTick();
}


void TIM1_BRK_UP_TRG_COM_IRQHandler(void) //Таймер для обработки кнопок
{
 
  
}











