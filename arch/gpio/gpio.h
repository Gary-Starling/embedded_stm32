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

#define GPIOD_AFL (*(volatile uint32_t *)(GPIOD_BASE + 0x20))
#define GPIOD_AFH (*(volatile uint32_t *)(GPIOD_BASE + 0x24))

typedef enum
{
    ON = 0,
    OFF = 1,
} eState;

void led_init(void);
void led_set_state(eState s);
void led_toggle(void);

#endif