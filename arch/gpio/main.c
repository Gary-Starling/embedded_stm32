#include "sys.h"
#include "gpio.h"
#include "pwm.h"

int main(void)
{
    flash_set_waitstates();
    clock_config();
    systick_enable();
    // led_init();
    led_pwm_init();
    pwm_init(CPU_FREQ, 0);
    while (1)
        WFI();
}

void SysTick_Handler(void)
{
    static uint32_t msec = 0;
    static uint32_t pwm = 0;

    msec++;

    if (msec % 100 == 0)
    {
        pwm_init(CPU_FREQ, pwm++);
        if (pwm >= 100)
            pwm = 0;
    }
}