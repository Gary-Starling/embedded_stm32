#include <stdio.h>
#include "sys.h"
#include "uart.h"
#include <string.h>

static volatile uint32_t del = 0;
static volatile int cnt = 0;
char gl_msg[256];

int main(void)
{
    char send_msg[64];
    flash_set_waitstates();
    clock_config();
    systick_enable();
    uart3_setup(115200, 8, 'N', 1);

    while (1)
    {

        // len = printf("MCU send count = %d\r\n", cnt);
        // can use printf to redirect uart
        if (del % 500 == 0)
        {
            cnt++;
            sprintf(send_msg, "Msg to PC N=%d", cnt);
            uart3_write_irq(send_msg, strlen(send_msg));
            uart3_read_irq(gl_msg, 20);
        }
    }
}

void SysTick_Handler(void)
{
    del++;
}