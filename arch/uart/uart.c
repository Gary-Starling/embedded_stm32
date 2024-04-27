#include "uart.h"
#include "sys.h"
#include "stddef.h"

#define RX_SIZE (256)
#define TX_SIZE (256)

volatile char rx_buff[RX_SIZE];
volatile uint32_t rx_p = 0;
volatile uint32_t unr_b = 0;

volatile char tx_buff[RX_SIZE];
volatile uint32_t tx_p = 0;
volatile uint32_t uns_p = 0;

/**
 * @brief
 *
 */
void uart3_pins_setup(void)
{
    AHB1_CLOCK_ER |= GPIOD_AHB1_CLOCK_ER;

    GPIOD_MODE &= ~(0x03 << (UART3_RX_PIN * 2));
    GPIOD_MODE |= (2 << (UART3_RX_PIN * 2));

    GPIOD_MODE &= ~(0x03 << (UART3_TX_PIN * 2));
    GPIOD_MODE |= (2 << (UART3_TX_PIN * 2));

    GPIOD_AFH &= ~(0xf << 0);
    GPIOD_AFH |= (UART3_PIN_AF << 0);

    GPIOD_AFH &= ~(0xf << 4);
    GPIOD_AFH |= (UART3_PIN_AF << 4);
}

/**
 * @brief
 *
 * @param bitrate
 * @param data
 * @param parity
 * @param stop
 * @return int
 */
int uart3_setup(uint32_t bitrate, uint8_t data,
                char parity, uint8_t stop)
{
    uint32_t reg;

    uart3_pins_setup();

    /* Clock for uart3 */
    APB1_CLOCK_ER |= UART3_APB1_CLOCK_ER_VAL;
    /* TX and RX enable */
    UART3_CR1 |= (UART_CR1_TX_ENABLE | UART_CR1_RX_ENABLE);
    UART3_BRR = CPU_FREQ / bitrate;

    UART3_CR1 &= ~(UART_CR1_PARITY_ENABLED | UART_CR1_PARITY_ODD);
    switch (parity)
    {
    case 'O':
        UART3_CR1 |= UART_CR1_PARITY_ODD;
    /* parity */
    case 'E':
        UART3_CR1 |= UART_CR1_PARITY_ENABLED;
        break;

    default:
        break;
    }

    reg = UART3_CR2 & ~UART_CR2_STOPBITS;
    if (stop > 1)
        UART3_CR2 = reg | (2 << 12);

    /* Enable interrupts in NVIC */
    nvic_irq_enable(NVIC_UART3_IRQN);
    nvic_irq_setprio(NVIC_UART3_IRQN, 0);

    /* Enable RX interrupt */
    uart3rx_irq_on_off(1);

    UART3_CR1 |= UART_CR1_UART_ENABLE;
    return 0;
}

void uart3tx_irq_on_off(int en_dis)
{
    if (en_dis)
        UART3_CR1 |= UART_CR1_TXEIE;
    else
        UART3_CR1 &= ~UART_CR1_TXEIE;
}
void uart3rx_irq_on_off(int en_dis)
{
    if (en_dis)
        UART3_CR1 |= UART_CR1_RXNEIE;
    else
        UART3_CR1 &= ~UART_CR1_RXNEIE;
}

/**
 * @brief
 *
 * @param msg
 */
void uart3_send(const char *msg)
{
    while (*msg)
    {
        while ((UART3_SR & UART_SR_TX_EMPTY) == 0)
            ; // TODO: cnt for timeout
        UART3_DR = *msg;
        msg++;
    }
}

/**
 * @brief
 *
 * @return char
 */
char uart3_read_byte(void)
{
    volatile char c;
    volatile uint32_t reg = UART3_SR;
    uint32_t uart_timeout = 0;

    while ((reg & UART_SR_RX_NOTEMPTY) == 0)
        reg = UART3_SR;

    c = (char)(UART3_DR & 0xff);
    return c;
}

/**
 * @brief
 *
 * @param msg
 * @param len
 */
void uart3_read_msg(char *msg, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
        msg[i] = uart3_read_byte();
}

/**
 * @brief
 *
 * @param buf
 * @param len
 * @return int
 */
void uart3_read_irq(char *buf, int len)
{
    static uint32_t start = 0;
    uint32_t i;

    if (unr_b == 0)
        return; // nothing to do

    if (buf == NULL)
        return;

    for (i = 0; i < unr_b; i++)
    {
        buf[i] = rx_buff[start];
        start++;
        start %= RX_SIZE;
    }

    unr_b -= i;
}

void uart3_write_irq(const char *msg, int len)
{

    if (len <= 0)
        return;
    if (len >= TX_SIZE)
        len = TX_SIZE - 1;
    if (msg == NULL)
        return;

    while (uns_p != 0)
        ;
    while ((UART3_SR & UART_SR_TX_EMPTY) == 0)
        ;
    UART3_DR = 0x00;

    for (uint32_t i = 0; i < len; i++)
    {
        tx_buff[i] = *msg;
        msg++;
    }
    uns_p = len;

    uart3tx_irq_on_off(1);
}

/**
 * @brief
 *
 * @param ch
 * @return int
 */
int __io_putchar(int ch)
{
    while ((UART3_SR & UART_SR_TX_EMPTY) == 0)
        ;
    UART3_DR = (uint32_t)ch;

    return ch;
}

void USART3_IRQHandler(void)
{
    volatile uint32_t reg = UART3_SR;

    /* rx byte */
    if (reg & UART_SR_RX_NOTEMPTY)
    {
        rx_buff[rx_p % RX_SIZE] = (char)(UART3_DR & 0xFF);
        rx_p++;
        unr_b++;
    }

    /* end of transmit */
    if ((reg & UART_SR_TX_EMPTY))
    {
        UART3_DR = tx_buff[tx_p++];

        if (tx_p > TX_SIZE)
            tx_p = 0;

        if (uns_p != 0)
            uns_p--;

        if (uns_p == 0)
        {
            UART3_DR = 0x00;
            tx_p = 0;
            uart3tx_irq_on_off(0);
        }
    }
}
