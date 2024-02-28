#ifndef _SYS_H_
#define _SYS_H_

#include <stdint.h>

#define SYSTICK_BASE (0xE000E010)
#define SYSTICK_CSR (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_RVR (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_CVR (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))
#define SYSTICK_CALIB (*(volatile uint32_t *)(SYSTICK_BASE + 0x0C))

/* System specific: PLL with 8 MHz external oscillator, CPU at 168MHz */
#define CPU_FREQ (168000000)
#define PLL_FULL_MASK (0x7F037FFF)

/* NVIC */
/* NVIC ISER Base register (Cortex-M) */

#define NVIC_TIM2_IRQN        (28)
#define NVIC_ISER_BASE (0xE000E100)
#define NVIC_ICER_BASE (0xE000E180)
#define NVIC_IPRI_BASE (0xE000E400)

/* Assembly helpers */
#define DMB() __asm__ volatile ("dmb");
#define WFI() __asm__ volatile ("wfi");

static inline void nvic_irq_enable(uint8_t n)
{
    int i = n / 32;
    volatile uint32_t *nvic_iser = ((volatile uint32_t *)(NVIC_ISER_BASE + 4 * i));
    *nvic_iser |= (1 << (n % 32));
}

static inline void nvic_irq_disable(uint8_t n)
{
    int i = n / 32;
    volatile uint32_t *nvic_icer = ((volatile uint32_t *)(NVIC_ICER_BASE + 4 * i));
    *nvic_icer |= (1 << (n % 32));
}

static inline void nvic_irq_setprio(uint8_t n, uint8_t prio)
{
    volatile uint8_t *nvic_ipri = ((volatile uint8_t *)(NVIC_IPRI_BASE + n));
    *nvic_ipri = prio;
}


/*** FLASH ***/
#define FLASH_BASE (0x40023C00)
#define FLASH_ACR (*(volatile uint32_t *)(FLASH_BASE + 0x00))
#define FLASH_ACR_ENABLE_DATA_CACHE (1 << 10)
#define FLASH_ACR_ENABLE_INST_CACHE (1 << 9)

/*** RCC ***/

#define RCC_BASE (0x40023800)
#define RCC_CR (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_CR (*(volatile uint32_t *)(RCC_BASE + 0x00))

#define RCC_CR_PLLRDY (1 << 25)
#define RCC_CR_PLLON (1 << 24)
#define RCC_CR_HSERDY (1 << 17)
#define RCC_CR_HSEON (1 << 16)
#define RCC_CR_HSIRDY (1 << 1)
#define RCC_CR_HSION (1 << 0)

#define RCC_CFGR_SW_HSI 0x0
#define RCC_CFGR_SW_HSE 0x1
#define RCC_CFGR_SW_PLL 0x2

#define RCC_PLLCFGR_PLLSRC (1 << 22)

#define RCC_PRESCALER_DIV_NONE 0
#define RCC_PRESCALER_DIV_2 8
#define RCC_PRESCALER_DIV_4 9

#define PLLM 8
#define PLLN 336
#define PLLP 2
#define PLLQ 7
#define PLLR 0


void flash_set_waitstates(void);
void clock_config(void);
void systick_enable(void);

#endif