#include <stdint.h>
#include "testlib.h"

void main(void)
{
    uint32_t *pClkCtrlReg = (uint32_t *)0x40023830;
    uint32_t *pPortDModeReg = (uint32_t *)0x40020C00;
    uint32_t *pPortDOutReg = (uint32_t *)0x40020C14;

    // This code turns on the all 4 user LEDs

    // Enables peripheral clock register for GPIOD (Port D)
    *pClkCtrlReg |= (1 << 3);

    // Sets pins 15, 14, 13, 12 as outputs
    *pPortDModeReg |= (1 << 30);
    *pPortDModeReg |= (1 << 28);
    *pPortDModeReg |= (1 << 26);
    *pPortDModeReg |= (1 << 24);

    // Writes a 1 to pins 15, 14, 13, 12 setting them HIGH to turn on LEDs
    *pPortDOutReg |= (1 << 15);
    *pPortDOutReg |= (1 << 14);
    *pPortDOutReg |= (1 << 13);
    *pPortDOutReg |= (1 << 12);

    int res = utils_open(0);
    res = utils_close(res);

    /* Loop forever */
    while (1)
        ;
}