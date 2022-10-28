#include <stdint.h>
#include <system_stm32f0xx.h>
#include <stm32f0xx.h>

extern uint32_t _estack;

extern void __libc_init_array();
extern int main(void);

void Reset_Handler(void)
{
  int res = 0;
  /* startup */
  extern uint8_t _sdata, _edata, _sidata,
      __bss_start__, __bss_end__;

  uint8_t *dst = &__bss_start__;

  //Обнуление bss
  while (dst < &__bss_end__)
    *dst++ = 0;

  dst = &_sdata;
  //запись в .data данных из flash
  uint8_t *src = &_sidata;

  while (dst < &_edata)
    *dst++ = *src++;

  __libc_init_array();
  res = main();

  while(1);
}

void Default_Handler(void)
{
  while (1)
    ;
}



__weak void NMI_Handler(void)                    { Default_Handler(); }
__weak void HardFault_Handler(void)              { Default_Handler(); }
__weak void SVC_Handler(void)                    { Default_Handler(); }
__weak void PendSV_Handler(void)                 { Default_Handler(); }
__weak void SysTick_Handler(void)                { Default_Handler(); }
__weak void WWDG_IRQHandler(void)                { Default_Handler(); }                /* Window WatchDog              */
__weak void RTC_IRQHandler(void)                 { Default_Handler(); }                /* RTC through the EXTI line    */
__weak void FLASH_IRQHandler(void)               { Default_Handler(); }                /* FLASH                        */
__weak void RCC_IRQHandler(void)                 { Default_Handler(); }                /* RCC                          */
__weak void EXTI0_1_IRQHandler(void)             { Default_Handler(); }                /* EXTI Line 0 and 1            */
__weak void EXTI2_3_IRQHandler(void)             { Default_Handler(); }                /* EXTI Line 2 and 3            */
__weak void EXTI4_15_IRQHandler(void)            { Default_Handler(); }                /* EXTI Line 4 to 15            */
__weak void DMA1_Channel1_IRQHandler(void)       { Default_Handler(); }                /* DMA1 Channel 1               */
__weak void DMA1_Channel2_3_IRQHandler(void)     { Default_Handler(); }                /* DMA1 Channel 2 and Channel 3 */
__weak void DMA1_Channel4_5_IRQHandler(void)     { Default_Handler(); }                /* DMA1 Channel 4 and Channel 5 */
__weak void ADC1_IRQHandler(void)                { Default_Handler(); }                /* ADC1                         */
__weak void TIM1_BRK_UP_TRG_COM_IRQHandler(void) { Default_Handler(); }                /* TIM1 Break, Update, Trigger and Commutation */
__weak void TIM1_CC_IRQHandler(void)             { Default_Handler(); }                /* TIM1 Capture Compare         */
__weak void TIM3_IRQHandler(void)                { Default_Handler(); }                /* TIM3                         */
__weak void TIM14_IRQHandler(void)               { Default_Handler(); }                /* TIM14                        */
__weak void TIM16_IRQHandler(void)               { Default_Handler(); }                /* TIM16                        */
__weak void TIM17_IRQHandler(void)               { Default_Handler(); }                /* TIM17                        */
__weak void I2C1_IRQHandler(void)                { Default_Handler(); }                /* I2C1                         */
__weak void SPI1_IRQHandler(void)                { Default_Handler(); }                /* SPI1                         */
__weak void USART1_IRQHandler(void)              { Default_Handler(); }                /* USART1                       */


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



