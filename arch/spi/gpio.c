#include "gpio.h"
#include "sys.h"

void led_init(void)
{
  AHB1_CLOCK_ER |= GPIOD_AHB1_CLOCK_ER;

  GPIOD_MODE &= ~(0x03 << (LED_PIN * 2));
  GPIOD_MODE |= 1 << (LED_PIN * 2);
  GPIOD_PUPD &= ~(0x03 << (LED_PIN * 2));
  GPIOD_PUPD |= 0x02 << (LED_PIN * 2);
}

void led_set_state(eState s)
{
  if (ON)
    GPIOD_BSRR |= 1 << LED_PIN;
  else
    GPIOD_BSRR |= 1 << (LED_PIN + 16);
}

void led_toggle(void)
{
  if ((GPIOD_ODR & (1 << LED_PIN)) == 0)
    led_set_state(ON);
  else
    led_set_state(OFF);
}

void button_setup(void)
{
  AHB1_CLOCK_ER |= GPIOA_AHB1_CLOCK_ER;
  GPIOA_MODE &= ~(0x03 << (BUTTON_PIN * 2));
}

int button_is_set(void)
{
  return (GPIOA_IDR & (1 << BUTTON_PIN)) >> BUTTON_PIN;
}

void button_exti_setup(void)
{
  AHB1_CLOCK_ER |= GPIOA_AHB1_CLOCK_ER;
  GPIOA_MODE &= ~(0x03 << (BUTTON_PIN * 2));
  EXTI_CR0 &= ~EXTI_CR_EXTI0_MASK;
  nvic_irq_enable(NVIC_EXTI0_IRQN);
  EXTI_IMR |= 1 << BUTTON_PIN;
  EXTI_EMR |= 1 << BUTTON_PIN;
  EXTI_RTSR |= 1 << BUTTON_PIN;
}