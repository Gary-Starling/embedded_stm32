#include "sys.h"

void flash_set_waitstates(void)
{
  FLASH_ACR |= 5 | FLASH_ACR_ENABLE_DATA_CACHE | FLASH_ACR_ENABLE_INST_CACHE;
}

void clock_config(void)
{
  uint32_t reg32;
  /* Enable internal high-speed oscillator. */
  RCC_CR |= RCC_CR_HSION;
  DMB();
  while ((RCC_CR & RCC_CR_HSIRDY) == 0)
  {
  };

  /* Select HSI as SYSCLK source. */

  reg32 = RCC_CFGR;
  reg32 &= ~((1 << 1) | (1 << 0));
  RCC_CFGR = (reg32 | RCC_CFGR_SW_HSI);
  DMB();

  /* Enable external high-speed oscillator 8MHz. */
  RCC_CR |= RCC_CR_HSEON;
  DMB();
  while ((RCC_CR & RCC_CR_HSERDY) == 0)
  {
  };

  /*
   * Set prescalers for AHB, ADC, ABP1, ABP2.
   */
  reg32 = RCC_CFGR;
  reg32 &= ~(0xF0);
  RCC_CFGR = (reg32 | (RCC_PRESCALER_DIV_NONE << 4));
  DMB();
  reg32 = RCC_CFGR;
  reg32 &= ~(0x1C00);
  RCC_CFGR = (reg32 | (RCC_PRESCALER_DIV_2 << 10));
  DMB();
  reg32 = RCC_CFGR;
  reg32 &= ~(0x07 << 13);
  RCC_CFGR = (reg32 | (RCC_PRESCALER_DIV_4 << 13));
  DMB();

  /* Set PLL config */
  reg32 = RCC_PLLCFGR;
  reg32 &= ~(PLL_FULL_MASK);
  RCC_PLLCFGR = reg32 | RCC_PLLCFGR_PLLSRC | PLLM |
                (PLLN << 6) | (((PLLP >> 1) - 1) << 16) |
                (PLLQ << 24);
  DMB();
  /* Enable PLL oscillator and wait for it to stabilize. */
  RCC_CR |= RCC_CR_PLLON;
  DMB();
  while ((RCC_CR & RCC_CR_PLLRDY) == 0)
  {
  };

  /* Select PLL as SYSCLK source. */
  reg32 = RCC_CFGR;
  reg32 &= ~((1 << 1) | (1 << 0));
  RCC_CFGR = (reg32 | RCC_CFGR_SW_PLL);
  DMB();

  /* Wait for PLL clock to be selected. */
  while ((RCC_CFGR & ((1 << 1) | (1 << 0))) != RCC_CFGR_SW_PLL)
  {
  };

  /* Disable internal high-speed oscillator. */
  RCC_CR &= ~RCC_CR_HSION;
}

void systick_enable(void)
{
  SYSTICK_RVR = ((CPU_FREQ / 1000) - 1);
  SYSTICK_CVR = 0;
  SYSTICK_CSR = (1 << 0) | (1 << 1) | (1 << 2);
}