#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

#define AHB1_CLOCK_ER (*(volatile uint32_t *)(0x40023830))
#define GPIOD_AHB1_CLOCK_ER (1 << 3)
#define GPIOD_BASE 0x40020c00
#define GPIOD_MODE (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIOD_AFL (*(volatile uint32_t *)(GPIOD_BASE + 0x20))
#define GPIOD_AFH (*(volatile uint32_t *)(GPIOD_BASE + 0x24))
#define GPIO_MODE_AF (2)
#define UART3_PIN_AF (7)
#define UART3_RX_PIN (9)
#define UART3_TX_PIN (8)


#define APB1_CLOCK_ER (*(volatile uint32_t *)(0x40023840))
#define UART3_APB1_CLOCK_ER_VAL (1 << 18)

#define UART3 (0x40004800)
#define UART3_SR (*(volatile uint32_t *)(UART3))
#define UART3_DR (*(volatile uint32_t *)(UART3 + 0x04))
#define UART3_BRR (*(volatile uint32_t *)(UART3 + 0x08))
#define UART3_CR1 (*(volatile uint32_t *)(UART3 + 0x0c))
#define UART3_CR2 (*(volatile uint32_t *)(UART3 + 0x10))

#define UART_CR1_UART_ENABLE (1 << 13)
#define UART_CR1_SYMBOL_LEN (1 << 12)
#define UART_CR1_PARITY_ENABLED (1 << 10)
#define UART_CR1_PARITY_ODD (1 << 9)
#define UART_CR1_TX_ENABLE (1 << 3)
#define UART_CR1_RX_ENABLE (1 << 2)
#define UART_CR2_STOPBITS (3 << 12)
#define UART_SR_TX_EMPTY (1 << 7)
#define UART_SR_RX_NOTEMPTY (1 << 5)

#define UART_CR1_TXEIE (1 << 7)
#define UART_CR1_RXNEIE (1 << 5)

#define NVIC_UART3_IRQN          (39)

void uart3_pins_setup(void);

int uart3_setup(uint32_t bitrate, uint8_t data,
                char parity, uint8_t stop);

void uart3tx_irq_on_off(int en_dis);

void uart3rx_irq_on_off(int en_dis);

void uart3_send(const char *msg);

char uart3_read_byte(void);

void uart3_read_msg(char *msg, uint32_t len);

void uart3_read_irq(char *buf, int len);

void uart3_write_irq(const char *msg, int len);

#endif