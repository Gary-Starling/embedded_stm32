#include "sys.h"

int main(void)
{
    flash_set_waitstates();
    clock_config();
    systick_enable();
    while (1)
        ;
}

volatile unsigned int test = 0;
void SysTick_Handler(void)
{
    test++;
}
