#include <stdint.h>

extern unsigned int _stored_data;
extern unsigned int _start_data;
extern unsigned int _end_data;
extern unsigned int _start_bss;
extern unsigned int _end_bss;

extern void *END_STACK;

#define APP_OFFSET (0x08004000)
#define VTOR (*(volatile uint32_t *)(0xE000ED08))

void isr_reset(void)
{
    const uint32_t *const app_vectors = (const uint32_t *)APP_OFFSET;
    unsigned int *src, *dst;
    void *app_entry;
    uint32_t app_end_stack;

    /* Copy the .data section from flash to RAM. */
    src = (unsigned int *)&_stored_data;
    dst = (unsigned int *)&_start_data;
    while (dst < (unsigned int *)&_end_data)
    {
        *dst = *src;
        dst++;
        src++;
    }

    /* Initialize the BSS section to 0 */
    dst = &_start_bss;
    while (dst < (unsigned int *)&_end_bss)
    {
        *dst = 0U;
        dst++;
    }

    asm volatile("cpsid i");                               /* Disable interrupts */
    VTOR = (uint32_t)app_vectors;                          /* New vector table */
    app_end_stack = (*((uint32_t *)(APP_OFFSET)));         /* Addr stack */
    app_entry = (void *)(*((uint32_t *)(APP_OFFSET + 4))); /* Addr reset handler */

    asm volatile("msr msp, %0" ::"r"(app_end_stack)); /* Update stack pointer */
    asm volatile("mov pc, %0" ::"r"(app_entry));      /* Unconditionally jump to app_entry */
}

void ISR_empty(void)
{
}

void isr_fault(void)
{
    /* Panic. */
    while (1)
        ;
}

__attribute__((weak)) void PendSV_Handler(void) { ISR_empty(); }
__attribute__((weak)) void SysTick_Handler(void) { ISR_empty(); }
__attribute__((weak)) void WWDG_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void PVD_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TAMP_STAMP_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void RTC_WKUP_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void FLASH_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void RCC_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void EXTI0_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void EXTI1_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void EXTI2_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void EXTI3_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void EXTI4_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream0_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream1_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream2_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream3_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream4_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream5_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream6_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void ADC_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN1_TX_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN1_RX0_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN1_RX1_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN1_SCE_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void EXTI9_5_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM1_BRK_TIM9_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM1_UP_TIM10_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM1_TRG_COM_TIM11_IRQHand(void) { ISR_empty(); }
__attribute__((weak)) void TIM1_CC_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM2_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM3_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM4_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void I2C1_EV_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void I2C1_ER_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void I2C2_EV_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void I2C2_ER_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void SPI1_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void SPI2_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void USART1_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void USART2_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void USART3_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void EXTI15_10_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void RTC_Alarm_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void OTG_FS_WKUP_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM8_BRK_TIM12_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM8_UP_TIM13_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM8_TRG_COM_TIM14_IRQHand(void) { ISR_empty(); }
__attribute__((weak)) void TIM8_CC_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA1_Stream7_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void FSMC_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void SDIO_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM5_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void SPI3_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void UART4_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void UART5_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM6_DAC_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void TIM7_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream0_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream1_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream2_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream3_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream4_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN2_TX_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN2_RX0_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN2_RX1_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void CAN2_SCE_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void OTG_FS_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream5_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream6_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void DMA2_Stream7_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void USART6_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void I2C3_EV_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void I2C3_ER_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void OTG_HS_EP1_OUT_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void OTG_HS_EP1_IN_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void OTG_HS_WKUP_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void OTG_HS_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void HASH_RNG_IRQHandler(void) { ISR_empty(); }
__attribute__((weak)) void FPU_IRQHandler(void) { ISR_empty(); }

__attribute__((section(".isr_vector"))) void (*const vector[])(void) =
    {
        (void (*)(void))(&END_STACK),
        isr_reset,
        isr_fault,
        isr_fault,
        isr_fault,
        isr_fault,
        isr_fault,
        0,
        0,
        0,
        0,
        isr_fault,
        isr_fault,
        0,
        PendSV_Handler,
        SysTick_Handler,
        WWDG_IRQHandler,
        PVD_IRQHandler,
        TAMP_STAMP_IRQHandler,
        RTC_WKUP_IRQHandler,
        FLASH_IRQHandler,
        RCC_IRQHandler,
        EXTI0_IRQHandler,
        EXTI1_IRQHandler,
        EXTI2_IRQHandler,
        EXTI3_IRQHandler,
        EXTI4_IRQHandler,
        DMA1_Stream0_IRQHandler,
        DMA1_Stream1_IRQHandler,
        DMA1_Stream2_IRQHandler,
        DMA1_Stream3_IRQHandler,
        DMA1_Stream4_IRQHandler,
        DMA1_Stream5_IRQHandler,
        DMA1_Stream6_IRQHandler,
        ADC_IRQHandler,
        CAN1_TX_IRQHandler,
        CAN1_RX0_IRQHandler,
        CAN1_RX1_IRQHandler,
        CAN1_SCE_IRQHandler,
        EXTI9_5_IRQHandler,
        TIM1_BRK_TIM9_IRQHandler,
        TIM1_UP_TIM10_IRQHandler,
        TIM1_TRG_COM_TIM11_IRQHand,
        TIM1_CC_IRQHandler,
        TIM2_IRQHandler,
        TIM3_IRQHandler,
        TIM4_IRQHandler,
        I2C1_EV_IRQHandler,
        I2C1_ER_IRQHandler,
        I2C2_EV_IRQHandler,
        I2C2_ER_IRQHandler,
        SPI1_IRQHandler,
        SPI2_IRQHandler,
        USART1_IRQHandler,
        USART2_IRQHandler,
        USART3_IRQHandler,
        EXTI15_10_IRQHandler,
        RTC_Alarm_IRQHandler,
        OTG_FS_WKUP_IRQHandler,
        TIM8_BRK_TIM12_IRQHandler,
        TIM8_UP_TIM13_IRQHandler,
        TIM8_TRG_COM_TIM14_IRQHand,
        TIM8_CC_IRQHandler,
        DMA1_Stream7_IRQHandler,
        FSMC_IRQHandler,
        SDIO_IRQHandler,
        TIM5_IRQHandler,
        SPI3_IRQHandler,
        UART4_IRQHandler,
        UART5_IRQHandler,
        TIM6_DAC_IRQHandler,
        TIM7_IRQHandler,
        DMA2_Stream0_IRQHandler,
        DMA2_Stream1_IRQHandler,
        DMA2_Stream2_IRQHandler,
        DMA2_Stream3_IRQHandler,
        DMA2_Stream4_IRQHandler,
        0,
        0,
        CAN2_TX_IRQHandler,
        CAN2_RX0_IRQHandler,
        CAN2_RX1_IRQHandler,
        CAN2_SCE_IRQHandler,
        OTG_FS_IRQHandler,
        DMA2_Stream5_IRQHandler,
        DMA2_Stream6_IRQHandler,
        DMA2_Stream7_IRQHandler,
        USART6_IRQHandler,
        I2C3_EV_IRQHandler,
        I2C3_ER_IRQHandler,
        OTG_HS_EP1_OUT_IRQHandler,
        OTG_HS_EP1_IN_IRQHandler,
        OTG_HS_WKUP_IRQHandler,
        OTG_HS_IRQHandler,
        0,
        0,
        HASH_RNG_IRQHandler,
        FPU_IRQHandler};