#include "mpu.h"

static void recur_fail(void);

int main(void)
{
    int y = 0;

    mpu_enable();
    //recur_fail();

    while (1)
    {        
        y++;
        y += 10;
        y *= 2;
    }
}

static void recur_fail(void)
{
    static int i = 0;
    
    i++;
    recur_fail();
}