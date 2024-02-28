#include "sys.h"
#include "gpio.h"

int main(void)
{
    flash_set_waitstates();
    clock_config();
    systick_enable();
    led_init();
    while (1)
        WFI();
}

void SysTick_Handler(void)
{
    static uint32_t msec = 0;

    if (msec % 1000 == 0)
        led_toggle();
}