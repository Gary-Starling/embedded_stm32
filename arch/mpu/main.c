#include "mpu.h"

static void recur_fail(void);

void main(void)
{

    mpu_enable();
    recur_fail();

    while (1)
        ;
}

static void recur_fail(void)
{
    static int i = 0;
    
    i++;
    recur_fail();
}