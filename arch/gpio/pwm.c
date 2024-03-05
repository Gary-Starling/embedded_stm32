#include "gpio.h"
#include "pwm.h"

int pwm_init(uint32_t clock, uint32_t dutycycle)
{
    uint32_t val = (clock / 100000); /* Frequency is 100 KHz */
    uint32_t lvl;

    if (dutycycle > 100)
        return -1;

    lvl = (val * dutycycle) / 100;
    if (lvl != 0)
        lvl--;

    APB1_CLOCK_RST |= TIM4_APB1_CLOCK_ER_VAL;
    __asm__ volatile("dmb");
    APB1_CLOCK_RST &= ~TIM4_APB1_CLOCK_ER_VAL;
    APB1_CLOCK_ER |= TIM4_APB1_CLOCK_ER_VAL;

    /* disable CC */
    TIM4_CCER &= ~TIM_CCER_CC4_ENABLE;
    TIM4_CR1 = 0;
    TIM4_PSC = 0;
    TIM4_ARR = val - 1;
    TIM4_CCR4 = lvl;
    TIM4_CCMR1 &= ~(0x03 << 0);
    TIM4_CCMR1 &= ~(0x07 << 4);
    TIM4_CCMR1 |= TIM_CCMR1_OC1M_PWM1;
    TIM4_CCMR2 &= ~(0x03 << 8);
    TIM4_CCMR2 &= ~(0x07 << 12);
    TIM4_CCMR2 |= TIM_CCMR2_OC4M_PWM1;
    TIM4_CCER |= TIM_CCER_CC4_ENABLE;
    TIM4_CR1 |= TIM_CR1_CLOCK_ENABLE | TIM_CR1_ARPE;
    __asm__ volatile("dmb");
    return 0;
}

void led_pwm_init(void)
{
    AHB1_CLOCK_ER |= GPIOD_AHB1_CLOCK_ER;
    GPIOD_MODE &= ~(0x03 << (LED_PIN * 2));
    GPIOD_MODE |= (2 << (LED_PIN * 2));
    GPIOD_OSPD &= ~(0x03 << (LED_PIN * 2));
    GPIOD_OSPD |= (0x03 << (LED_PIN * 2));
    GPIOD_PUPD &= ~(0x03 << (LED_PIN * 2));
    GPIOD_PUPD |= (0x02 << (LED_PIN * 2));
    GPIOD_AFH &= ~(0xf << ((LED_PIN - 8) * 4));
    GPIOD_AFH |= (0x2 << ((LED_PIN - 8) * 4));
}