#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

#define SPI1 (0x40013000)
#define SPI1_CR1 (*(volatile uint32_t *)(SPI1))
#define SPI1_CR2 (*(volatile uint32_t *)(SPI1 + 0x04))
#define SPI1_SR (*(volatile uint32_t *)(SPI1 + 0x08))
#define SPI1_DR (*(volatile uint32_t *)(SPI1 + 0x0c))


#define SPI_CR1_MASTER (1 << 2)
#define SPI_CR1_SPI_EN (1 << 6)
#define SPI_CR2_SSOE (1 << 2)
#define SPI_SR_RX_NOTEMPTY (1 << 0)
#define SPI_SR_TX_EMPTY (1 << 1)

#define APB2_CLOCK_ER (*(volatile uint32_t *)(0x40023844))
#define APB2_CLOCK_RST (*(volatile uint32_t *)(0x40023824))
#define SPI1_APB2_CLOCK_ER_VAL (1 << 12)
#define GPIOE_AHB1_CLOCK_ER (1 << 4)

#define SPI1_PIN_AF 5
#define SPI1_CLOCK_PIN 5
#define SPI1_MOSI_PIN 6
#define SPI1_MISO_PIN 7

#define GPIOA_AFL   (*(volatile uint32_t *)(GPIOA_BASE + 0x20))
#define GPIOA_AFH   (*(volatile uint32_t *)(GPIOA_BASE + 0x24))
#define GPIO_MODE_AF (2)


#define PE3_PIN  3
#define GPIOE_BASE 0x40021000
#define GPIOE_MODE  (*(volatile uint32_t *)(GPIOE_BASE + 0x00))
#define GPIOE_OSPD  (*(volatile uint32_t *)(GPIOE_BASE + 0x08))
#define GPIOE_PUPD (*(volatile uint32_t *)(GPIOE_BASE + 0x0c))
#define GPIOE_BSRR (*(volatile uint32_t *)(GPIOE_BASE + 0x18))
#define GPIO_MODE_AF (2)

void spi1_reset(void);

void spi1_pins_setup(void);

void spi1_reset(void);

void spi1_setup(int polarity, int phase);

void spi1_write(const uint8_t c);
uint8_t spi1_read(void);

void slave_off(void);

void slave_on(void);

#endif