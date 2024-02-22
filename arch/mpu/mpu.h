#ifndef _MPU_H_H
#define _MPU_H_H

#include <stdint.h>

#define MPU_BASE (0xE000ED90)


/* FAULT enable register SHCSR */
#define SHCSR (*(volatile uint32_t *)(0xE000ED24))

/* Bit to enable memory fault handler */
#define MEMFAULT_ENABLE (1 << 16)

/* MPU Registers */
#define MPU_TYPE (*(volatile uint32_t *)(MPU_BASE + 0x00))
#define MPU_CTRL (*(volatile uint32_t *)(MPU_BASE + 0x04))
#define MPU_RNR  (*(volatile uint32_t *)(MPU_BASE + 0x08))
#define MPU_RBAR (*(volatile uint32_t *)(MPU_BASE + 0x0c))
#define MPU_RASR (*(volatile uint32_t *)(MPU_BASE + 0x10))

/* Some use-case specific values for RASR */
#define RASR_ENABLED  (1)
#define RASR_RW       (1 << 24)
#define RASR_RDONLY   (5 << 24)
#define RASR_NOACCESS (0 << 24)
#define RASR_SCB      (7 << 16)
#define RASR_SB       (5 << 16)
#define RASR_NOEXEC   (1 << 28)


/* Size */
#define MPUSIZE_1K      (0x09 << 1)
#define MPUSIZE_2K      (0x0a << 1)
#define MPUSIZE_4K      (0x0b << 1)
#define MPUSIZE_8K      (0x0c << 1)
#define MPUSIZE_16K     (0x0d << 1)
#define MPUSIZE_32K     (0x0e << 1)
#define MPUSIZE_64K     (0x0f << 1)
#define MPUSIZE_128K    (0x10 << 1)
#define MPUSIZE_256K    (0x11 << 1)
#define MPUSIZE_512K    (0x12 << 1)
#define MPUSIZE_1M      (0x13 << 1)
#define MPUSIZE_2M      (0x14 << 1)
#define MPUSIZE_4M      (0x15 << 1)
#define MPUSIZE_8M      (0x16 << 1)
#define MPUSIZE_16M     (0x17 << 1)
#define MPUSIZE_32M     (0x18 << 1)
#define MPUSIZE_64M     (0x19 << 1)
#define MPUSIZE_128M    (0x1a << 1)
#define MPUSIZE_256M    (0x1b << 1)
#define MPUSIZE_512M    (0x1c << 1)
#define MPUSIZE_1G      (0x1d << 1)
#define MPUSIZE_2G      (0x1e << 1)
#define MPUSIZE_4G      (0x1f << 1)


#define STACK_SIZE (4 * 1024)

void mpu_set_region(int region, uint32_t start, uint32_t attr);
int mpu_enable(void);


#endif