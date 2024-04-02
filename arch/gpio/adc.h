#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

#define APB2_CLOCK_ER (*(volatile uint32_t *)(0x40023844))
#define ADC1_APB2_CLOCK_ER_VAL (1 << 8)
#define ADC1_BASE (0x40012000)
#define ADC1_SR (*(volatile uint32_t *)(ADC1_BASE + 0x00))
#define ADC1_CR1 (*(volatile uint32_t *)(ADC1_BASE + 0x04))
#define ADC1_CR2 (*(volatile uint32_t *)(ADC1_BASE + 0x08))
#define ADC1_SMPR1 (*(volatile uint32_t *)(ADC1_BASE + 0x0c))
#define ADC1_SMPR2 (*(volatile uint32_t *)(ADC1_BASE + 0x10))
#define ADC1_SQR3 (*(volatile uint32_t *)(ADC1_BASE + 0x34))
#define ADC1_DR (*(volatile uint32_t *)(ADC1_BASE + 0x4c))
#define ADC_CR1_SCAN (1 << 8)
#define ADC_CR2_EN (1 << 0)
#define ADC_CR2_CONT (1 << 1)
#define ADC_CR2_SWSTART (1 << 30)
#define ADC_SR_EOC (1 << 1)
#define ADC_SMPR_SMP_480CYC (0x7)

#define AHB1_CLOCK_ER (*(volatile uint32_t *)(0x40023830))
#define GPIOB_AHB1_CLOCK_ER (1 << 1)
#define GPIOB_BASE (0x40020400)
#define GPIOB_MODE (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define ADC_PIN (1)
#define ADC_PIN_CHANNEL (9)

#define ADC_COM_BASE (0x40012300)
#define ADC_COM_CCR (*(volatile uint32_t *)(ADC_COM_BASE + 0x04))

void adc_init(void);
uint16_t adc_read(void);

#endif