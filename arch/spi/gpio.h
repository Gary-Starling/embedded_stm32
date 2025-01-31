#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>

// 0x40020C00 gpiod
#define LED_PIN (15)

#define AHB1_CLOCK_ER (*(volatile uint32_t *)(0x40023830))
#define GPIOD_AHB1_CLOCK_ER (1 << 3)

#define GPIOD_BASE 0x40020c00
#define GPIOD_MODE (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_OTYPE (*(volatile uint32_t *)(GPIOD_BASE + 0x04))
#define GPIOD_PUPD (*(volatile uint32_t *)(GPIOD_BASE + 0x0c))
#define GPIOD_ODR (*(volatile uint32_t *)(GPIOD_BASE + 0x14))
#define GPIOD_BSRR (*(volatile uint32_t *)(GPIOD_BASE + 0x18))
#define GPIOD_OSPD  (*(volatile uint32_t *)(GPIOD_BASE + 0x08))

#define GPIOD_AFL (*(volatile uint32_t *)(GPIOD_BASE + 0x20))
#define GPIOD_AFH (*(volatile uint32_t *)(GPIOD_BASE + 0x24))

#define GPIOA_BASE 0x40020000
#define GPIOA_MODE (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_IDR (*(volatile uint32_t *)(GPIOA_BASE + 0x10))
#define GPIOA_AHB1_CLOCK_ER (1 << 0)
#define BUTTON_PIN (0)

#define EXTI_CR_BASE (0x40013808)
#define EXTI_CR0 (*(volatile uint32_t *)(EXTI_CR_BASE + 0x00))
#define EXTI_CR_EXTI0_MASK (0x0F)

#define EXTI_BASE (0x40013C00)
#define EXTI_IMR (*(volatile uint32_t *)(EXTI_BASE + 0x00))

#define EXTI_EMR (*(volatile uint32_t *)(EXTI_BASE + 0x04))
#define EXTI_RTSR (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR (*(volatile uint32_t *)(EXTI_BASE + 0x0c))
#define EXTI_SWIER (*(volatile uint32_t *)(EXTI_BASE + 0x10))
#define EXTI_PR (*(volatile uint32_t *)(EXTI_BASE + 0x14))

#define NVIC_EXTI0_IRQN (6)

typedef enum
{
    ON = 0,
    OFF = 1,
} eState;

void led_init(void);
void led_set_state(eState s);
void led_toggle(void);
void button_setup(void);
int button_is_set(void);
void button_exti_setup(void);

#endif