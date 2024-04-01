#include "gpio.h"
#include "pwm.h"

#define PSC_PWM (0)

/**
 * @brief
 *
 * @param MCUclock
 * @param PWMclock
 * @param dutycycle
 * @return int
 * note
 *  Fpwm = Fclk/( (ARR+1) * (PSC + 1) )
 *  for 10kHz = 168Mhz/( (ARR+1) * (PSC + 1) )
 *  Duty % = CCRx/ARRx %
 */
int pwm_init(uint32_t MCUclock, uint32_t dutycycle, uint32_t PWMclock)
{
    uint32_t psc = PSC_PWM;
    uint32_t ARRx = (MCUclock / ((psc + 1) * (PWMclock))) - 1;
    uint32_t CCRx;

    while (ARRx > 65535)
    {
        psc++;
        ARRx = (MCUclock / ((psc + 1) * (PWMclock))) - 1;
    }

    if (dutycycle > 100)
        return -1;

    CCRx = (ARRx * dutycycle) / 100;

    if (CCRx != 0)
        CCRx--;

    // TODO: add code for calcilate PSC

    APB1_CLOCK_RST |= TIM4_APB1_CLOCK_ER_VAL;
    __asm__ volatile("dmb");
    APB1_CLOCK_RST &= ~TIM4_APB1_CLOCK_ER_VAL;
    APB1_CLOCK_ER |= TIM4_APB1_CLOCK_ER_VAL;

    /* disable CC */
    TIM4_CCER &= ~TIM_CCER_CC4_ENABLE;
    TIM4_CR1 = 0;
    TIM4_PSC = psc;
    TIM4_ARR = ARRx;
    TIM4_CCR4 = CCRx;
   // TIM4_CCMR1 &= ~(0x03 << 0);
   // TIM4_CCMR1 &= ~(0x07 << 4);
   // TIM4_CCMR1 |= TIM_CCMR1_OC1M_PWM1;
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