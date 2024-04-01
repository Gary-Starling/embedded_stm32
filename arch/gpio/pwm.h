#ifndef _PWM_H_
#define _PWM_H_

#define APB1_CLOCK_ER           (*(volatile uint32_t *)(0x40023840))
#define APB1_CLOCK_RST          (*(volatile uint32_t *)(0x40023820))
#define TIM4_APB1_CLOCK_ER_VAL 	(1 << 2)
#define TIM2_APB1_CLOCK_ER_VAL 	(1 << 0)


#define TIM4_BASE   (0x40000800)
#define TIM4_CR1    (*(volatile uint32_t *)(TIM4_BASE + 0x00))
#define TIM4_DIER   (*(volatile uint32_t *)(TIM4_BASE + 0x0c))
#define TIM4_SR     (*(volatile uint32_t *)(TIM4_BASE + 0x10))
#define TIM4_CCMR1  (*(volatile uint32_t *)(TIM4_BASE + 0x18))
#define TIM4_CCMR2  (*(volatile uint32_t *)(TIM4_BASE + 0x1c))
#define TIM4_CCER   (*(volatile uint32_t *)(TIM4_BASE + 0x20))
#define TIM4_PSC    (*(volatile uint32_t *)(TIM4_BASE + 0x28))
#define TIM4_ARR    (*(volatile uint32_t *)(TIM4_BASE + 0x2c))
#define TIM4_CCR4   (*(volatile uint32_t *)(TIM4_BASE + 0x40))

#define TIM_DIER_UIE (1 << 0)
#define TIM_SR_UIF   (1 << 0)
#define TIM_CR1_CLOCK_ENABLE (1 << 0)
#define TIM_CR1_UPD_RS       (1 << 2)
#define TIM_CR1_ARPE         (1 << 7)

#define TIM_CCER_CC4_ENABLE  (1 << 12)
#define TIM_CCMR1_OC1M_PWM1  (0x06 << 4)
#define TIM_CCMR2_OC4M_PWM1  (0x06 << 12)

int pwm_init(uint32_t MCUclock, uint32_t dutycycle, uint32_t PWMclock);
void led_pwm_init(void);

#endif