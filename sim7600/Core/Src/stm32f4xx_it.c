#include "main.h"
#include "stm32f4xx_it.h"
#include "RingBuff.h"


/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
void NMI_Handler(void)
{
  while (1);
}

void HardFault_Handler(void)
{
  while (1);
}

void MemManage_Handler(void)
{
  while (1);
}

void BusFault_Handler(void)
{
  while (1);
}

void UsageFault_Handler(void)
{
  while (1);
}


void DebugMon_Handler(void)
{

}



/**
* @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
*/
/* 1000Hz tim for hal driver*/
void TIM1_UP_TIM10_IRQHandler(void)
{

  /* TIM Update event */
  if( ((TIM1->SR & TIM_SR_UIF) == TIM_SR_UIF) && \
    ((TIM1->DIER & TIM_IT_UPDATE) == TIM_IT_UPDATE) )
  {
    TIM1->SR = ~TIM_IT_UPDATE;

    HAL_IncTick();
  }

}
