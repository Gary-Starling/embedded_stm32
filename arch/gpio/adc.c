#include "adc.h"

void adc_init(void)
{
    uint32_t val = ADC1_SMPR2;

    APB2_CLOCK_ER |= ADC1_APB2_CLOCK_ER_VAL;
    AHB1_CLOCK_ER |= GPIOB_AHB1_CLOCK_ER;

    GPIOB_MODE |= 0x03 << (ADC_PIN * 2);        //ain
    ADC1_CR2 &= ~(ADC_CR2_EN);                  //adc dis
    ADC_COM_CCR &= ~(0x03 << 16);               //psc
    ADC1_CR1 &= ~(ADC_CR1_SCAN);                //no scan
    ADC1_CR2 &= ~(ADC_CR2_CONT);                //one shot

    /* sample time */
    if (ADC_PIN_CHANNEL > 9)
        ADC1_SMPR2 = val;
    else
    {
        val = ADC_SMPR_SMP_480CYC << (ADC_PIN_CHANNEL * 3);
        ADC1_SMPR1 = val;
    }

    ADC1_SQR3 |= (ADC_PIN_CHANNEL);
    ADC1_CR2 |= ADC_CR2_EN;
}

uint16_t adc_read(void)
{
    uint16_t res;
    ADC1_CR2 |= ADC_CR2_SWSTART;
    while (ADC1_CR2 & ADC_CR2_SWSTART)
        ;
    while ((ADC1_SR & ADC_SR_EOC) == 0)
        ;

    res = ADC1_DR;
    return res;
}