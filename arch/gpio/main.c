#include "sys.h"
#include "gpio.h"
#include "pwm.h"

#define FPWM        (15000)

int main(void)
{
    flash_set_waitstates();
    clock_config();
    systick_enable();
    // led_init();
    led_pwm_init();
    pwm_init(CPU_FREQ, 10, FPWM);
    while (1)
        WFI();
}

void SysTick_Handler(void)
{
    static uint32_t msec = 0;
    static uint32_t duty = 0;

    msec++;

    if (msec % 100 == 0)
    {
        pwm_init(CPU_FREQ, duty++,FPWM);
        if (duty >= 100) duty = 0;
    }
}