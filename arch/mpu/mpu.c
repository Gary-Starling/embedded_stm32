#include "mpu.h"

extern uint32_t end_stack;

void mpu_set_region(int region, uint32_t start, uint32_t attr)
{
    MPU_RNR = region;
    MPU_RBAR = start;
    MPU_RNR = region;
    MPU_RASR = attr;
}

int mpu_enable(void)
{
    volatile uint32_t type;
    volatile uint32_t start;
    volatile uint32_t attr;

    type = MPU_TYPE;
      /* MPU not present! */
    if (type == 0)  return -1;

    /* Disable MPU to reconfigure regions */
    MPU_CTRL = 0;

    /* Set flash area as read-only, executable */
    start = 0;
    attr = RASR_ENABLED | MPUSIZE_256K | RASR_SCB | RASR_RDONLY;
    mpu_set_region(0, start, attr);

    /* Set RAM as read-write, not executable */
    start = 0x20000000;
    attr = RASR_ENABLED | MPUSIZE_64K | RASR_SCB | RASR_RW | RASR_NOEXEC;
    mpu_set_region(1, start, attr);

    /* Set 1KB guard area below the allowed stack region */
    start = (uint32_t)(&end_stack) - (STACK_SIZE + 1024);
    attr = RASR_ENABLED | MPUSIZE_1K | RASR_SCB | RASR_NOACCESS | RASR_NOEXEC;
    mpu_set_region(2, start, attr);

    /* Set System register area */
    start = 0xE0000000;
    attr = RASR_ENABLED | MPUSIZE_256M | RASR_SB | RASR_RW | RASR_NOEXEC;
    mpu_set_region(3, start, attr);
    
    /* Enable MEMFAULT */
    SHCSR |= MEMFAULT_ENABLE;

    /* Enable the MPU, no background region */
    MPU_CTRL = 1;
    return 0;

}