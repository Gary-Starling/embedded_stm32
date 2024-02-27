#include "tim.h"
#include "sys.h"

int timer_init(uint32_t clock, uint32_t interval_ms)
{
  uint32_t val = 0;
  uint32_t psc = 1;
  uint32_t err = 0;

  clock = (clock / 1000) * interval_ms;
  while (psc < 65535)
  {
    val = clock / psc;
    err = clock % psc;
    if ((val < 65535) && (err == 0))
    {
      val--;
      break;
    }
    val = 0;
    psc++;
  }
  if (val == 0)
    return -1;
  nvic_irq_enable(NVIC_TIM2_IRQN);
  nvic_irq_setprio(NVIC_TIM2_IRQN, 0);
  APB1_CLOCK_RST |= TIM2_APB1_CLOCK_ER_VAL;
  __asm__ volatile("dmb");
  APB1_CLOCK_RST &= ~TIM2_APB1_CLOCK_ER_VAL;
  APB1_CLOCK_ER |= TIM2_APB1_CLOCK_ER_VAL;

  TIM2_CR1 = 0;
  __asm__ volatile("dmb");
  TIM2_PSC = psc;
  TIM2_ARR = val;
  TIM2_CR1 |= TIM_CR1_CLOCK_ENABLE;
  TIM2_DIER |= TIM_DIER_UIE;
  __asm__ volatile("dmb");
  return 0;
}