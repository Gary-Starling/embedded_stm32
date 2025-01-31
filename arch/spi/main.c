#include "sys.h"
#include "spi.h"
#include <assert.h>

// LIS302D
// config
#define LIS302DL_WHO_AM_I_REG 0x0F
#define LIS302DL_CTRL_REG1 0x20
#define LIS302DL_CTRL_REG2 0x21
#define LIS302DL_CTRL_REG3 0x22
#define LIS302DL_HP_FILTER_RESET 0x23
#define LIS302DL_FF_WU_CFG_1 0x30
#define LIS302DL_FF_WU_SRC_1 0x31
#define LIS302DL_FF_WU_THS_1 0x32
#define LIS302DL_FF_WU_DURATION_1 0x33
#define LIS302DL_FF_WU_CFG_2 0x34
#define LIS302DL_FF_WU_SRC_2 0x35
#define LIS302DL_FF_WU_THS_2 0x36
#define LIS302DL_FF_WU_DURATION_2 0x37
#define LIS302DL_CLICK_CFG 0x38
#define LIS302DL_CLICK_SRC 0x39
#define LIS302DL_CLICK_THSY_X 0x3b
#define LIS302DL_CLICK_THSZ 0x3c
#define LIS302DL_CLICK_TimeLimit 0x3d
#define LIS302DL_CLICK_Latency 0x3e
#define LIS302DL_CLICK_Window 0x3f
// x y z
#define LIS302DL_OUT_X 0x29
#define LIS302DL_OUT_Y 0x2b
#define LIS302DL_OUT_Z 0x2d

typedef enum
{
    SET_BIT = 0,
    RESET_BIT,
} eMSbit;

static uint8_t LIS302D_read_pool(const uint8_t com, const eMSbit ms);

// bit 0: RW bit.When 0, the data DI(7:0) is written into the device. When 1, the data DO(7:0) from the device is read
// bit 1: MS bit. When 0, the address will remain unchanged in multiple read/write commands.When 1, the address will be auto incremented in multiple read/write commands.
// bit 2-7: address AD(5:0). This is the address field of the indexed register
// bit 8-15: data DI(7:0) (write mode). This is the data that will be written into the device (MSbfirst).
// bit 8-15: data DO(7:0) (read mode). This is the data that will be read from the device (MSbfirst).

int main(void)
{

    int err = 0;
    uint8_t b;

    flash_set_waitstates();
    clock_config();
    spi1_setup(0, 0);

    if ((b = LIS302D_read_pool(LIS302DL_WHO_AM_I_REG, RESET_BIT)) != 0x3b)
        err++;

    while (1)
        WFI();

    return 0;
}

static uint8_t LIS302D_read_pool(const uint8_t com, const eMSbit ms)
{
    uint8_t res;
    uint8_t sb = com | (1 << 7) | (6 << ms); // read bit is set, MS bit. When 0 do not increment address

    slave_on();        // cs on
    spi1_write(sb);    // write command
    res = spi1_read(); // nop
    spi1_write(0xFF);  // empty byte for clock
    res = spi1_read(); // read data
    slave_off();       // cs off

    return res;
}

static uint8_t LIS302D_write_pool(const uint8_t com, const eMSbit ms)
{
    uint8_t res;
    uint8_t sb = com;

    sb &= ~(1 << 7); // read bit is reset
    sb |= (6 << ms); // MS bit. When 0 do not increment address

    slave_on();        // cs on
    spi1_write(sb);    // write command
    res = spi1_read(); // nop
    spi1_write(0xFF);  // empty byte for clock
    res = spi1_read(); // read data
    slave_off();       // cs off

    return res;
}