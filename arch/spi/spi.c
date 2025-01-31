#include "spi.h"
#include "sys.h"
#include "gpio.h"

void spi1_reset(void)
{
    APB2_CLOCK_RST |= SPI1_APB2_CLOCK_ER_VAL;
    APB2_CLOCK_RST &= ~SPI1_APB2_CLOCK_ER_VAL;
}

void spi1_pins_setup(void)
{

    AHB1_CLOCK_ER |= GPIOA_AHB1_CLOCK_ER;

    /* spi pins */
    GPIOA_MODE &= ~(0x03 << (SPI1_CLOCK_PIN * 2));
    GPIOA_MODE &= ~(0x03 << (SPI1_MOSI_PIN));
    GPIOA_MODE &= ~(0x03 << (SPI1_MISO_PIN));
    GPIOA_MODE |= (2 << (SPI1_CLOCK_PIN * 2));
    GPIOA_MODE |= (2 << (SPI1_MOSI_PIN * 2)) | (2 << (SPI1_MISO_PIN * 2));

    GPIOA_AFL &= ~(0xf << ((SPI1_CLOCK_PIN) * 4));
    GPIOA_AFL &= ~(0xf << ((SPI1_MOSI_PIN) * 4));
    GPIOA_AFL &= ~(0xf << ((SPI1_MISO_PIN) * 4));
    GPIOA_AFL |= SPI1_PIN_AF << ((SPI1_CLOCK_PIN) * 4);
    GPIOA_AFL |= SPI1_PIN_AF << ((SPI1_MOSI_PIN) * 4);
    GPIOA_AFL |= SPI1_PIN_AF << ((SPI1_MISO_PIN) * 4);

    /* pe3 */
    AHB1_CLOCK_ER |= GPIOE_AHB1_CLOCK_ER;
    GPIOE_MODE &= ~(0x03 << (PE3_PIN * 2));
    GPIOE_MODE |= (1 << (PE3_PIN * 2));
    GPIOE_PUPD &= ~(0x03 << (PE3_PIN * 2));
    GPIOE_PUPD |= (0x01 << (PE3_PIN * 2));
    GPIOE_OSPD &= ~(0x03 << (PE3_PIN * 2));
    GPIOE_OSPD |= (0x03 << (PE3_PIN * 2));
}

void spi1_setup(int polarity, int phase)
{
    spi1_pins_setup();
    APB2_CLOCK_ER |= SPI1_APB2_CLOCK_ER_VAL;
    spi1_reset();
    SPI1_CR1 = SPI_CR1_MASTER | (5 << 3) | (polarity << 1) | (phase << 0);
    SPI1_CR2 |= SPI_CR2_SSOE;
    SPI1_CR1 |= SPI_CR1_SPI_EN;
    slave_off();
}

uint8_t spi1_read(void)
{
    volatile uint32_t reg;

    do
    {
        reg = SPI1_SR;
        // TODO: timeoout
    } while ((reg & SPI_SR_RX_NOTEMPTY) == 0);

    return (uint8_t)SPI1_DR;
}

void spi1_write(const uint8_t c)
{
    volatile uint32_t reg;
    SPI1_DR = c;
    do
    {
        // TODO: timeout
        reg = SPI1_SR;
    } while ((reg & SPI_SR_TX_EMPTY) == 0);
}

void slave_off(void)
{
    GPIOE_BSRR |= (1 << PE3_PIN);
}

void slave_on(void)
{
    GPIOE_BSRR |= (1 << (PE3_PIN + 16));
}