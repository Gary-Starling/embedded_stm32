#include <stdint.h>

extern uint32_t _estack;



void Reset_Handler(void) __attribute__((weak, alias("Default_Handler")));
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));                /* Window WatchDog              */
void RTC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));                 /* RTC through the EXTI line    */
void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));               /* FLASH                        */
void RCC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));                 /* RCC                          */
void EXTI0_1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));             /* EXTI Line 0 and 1            */
void EXTI2_3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));             /* EXTI Line 2 and 3            */
void EXTI4_15_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));            /* EXTI Line 4 to 15            */
void DMA1_Channel1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));       /* DMA1 Channel 1               */
void DMA1_Channel2_3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));     /* DMA1 Channel 2 and Channel 3 */
void DMA1_Channel4_5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));     /* DMA1 Channel 4 and Channel 5 */
void ADC1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));                /* ADC1                         */
void TIM1_BRK_UP_TRG_COM_IRQHandler(void) __attribute__((weak, alias("Default_Handler"))); /* TIM1 Break, Update, Trigger and Commutation */
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));             /* TIM1 Capture Compare         */
void TIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));                /* TIM3                         */
void TIM14_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));               /* TIM14                        */
void TIM16_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));               /* TIM16                        */
void TIM17_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));               /* TIM17                        */
void I2C1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));                /* I2C1                         */
void SPI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));                /* SPI1                         */
void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));              /* USART1                       */


typedef void (*pFunc)(void);

const pFunc vectors[] __attribute__((section(".isr_vector"))) = {
  (pFunc)&_estack,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  SVC_Handler,
  0,
  0,
  PendSV_Handler,
  SysTick_Handler,
  WWDG_IRQHandler,                /* Window WatchDog              */
  0,                              /* Reserved                     */
  RTC_IRQHandler,                 /* RTC through the EXTI line    */
  FLASH_IRQHandler,               /* FLASH                        */
  RCC_IRQHandler,                 /* RCC                          */
  EXTI0_1_IRQHandler,             /* EXTI Line 0 and 1            */
  EXTI2_3_IRQHandler,             /* EXTI Line 2 and 3            */
  EXTI4_15_IRQHandler,            /* EXTI Line 4 to 15            */
  0,                              /* Reserved                     */
  DMA1_Channel1_IRQHandler,       /* DMA1 Channel 1               */
  DMA1_Channel2_3_IRQHandler,     /* DMA1 Channel 2 and Channel 3 */
  DMA1_Channel4_5_IRQHandler,     /* DMA1 Channel 4 and Channel 5 */
  ADC1_IRQHandler,                /* ADC1                         */
  TIM1_BRK_UP_TRG_COM_IRQHandler, /* TIM1 Break, Update, Trigger and Commutation */
  TIM1_CC_IRQHandler,             /* TIM1 Capture Compare         */
  0,                              /* Reserved                     */
  TIM3_IRQHandler,                /* TIM3                         */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  TIM14_IRQHandler,               /* TIM14                        */
  0,                              /* Reserved                     */
  TIM16_IRQHandler,               /* TIM16                        */
  TIM17_IRQHandler,               /* TIM17                        */
  I2C1_IRQHandler,                /* I2C1                         */
  0,                              /* Reserved                     */
  SPI1_IRQHandler,                /* SPI1                         */
  0,                              /* Reserved                     */
  USART1_IRQHandler,              /* USART1                       */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
  0,                              /* Reserved                     */
};

void Default_Handler(void)
{
  while (1)
    ;
}

