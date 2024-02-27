#include "tim.h"
#include "sys.h"
volatile unsigned int test = 0;

int main(void)
{
    flash_set_waitstates();
    clock_config();
    // systick_enable();
    timer_init(CPU_FREQ, 1000);
    while (1)
        WFI();
}

void TIM2_IRQHandler(void)
{
    TIM2_SR &= ~TIM_SR_UIF;
    test++;
}