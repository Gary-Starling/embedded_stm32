#include "sys.h"
#include "gpio.h"
#include "pwm.h"
#include "adc.h"

volatile uint32_t Fpwm = 1000;
volatile uint16_t Adc;

int main(void)
{
    flash_set_waitstates();
    clock_config();
    systick_enable();
    // led_init();
    led_pwm_init();
    pwm_init(CPU_FREQ, 10, Fpwm);
    // button_setup();
    button_exti_setup();
    adc_init();

    while (1)
    {
        Adc = adc_read();
    }
}

void SysTick_Handler(void)
{
    static uint32_t msec = 0;
    static uint32_t duty = 0;

    msec++;

    if (msec % 10 == 0)
    {
        pwm_init(CPU_FREQ, duty++, Fpwm);
        if (duty >= 100)
            duty = 0;
    }
}

void EXTI0_IRQHandler(void)
{
    EXTI_PR |= 1 << BUTTON_PIN;
    Fpwm = (Fpwm < 10000) ? Fpwm + 1000 : 1000;
}