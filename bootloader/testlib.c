#include "testlib.h"


int open(int d)
{
    return 1;
}

int close(int d)
{
     return -1;   
}

int write(int d, void * buff, unsigned int size)
{
    return 0;
}

int read(int d, void *buf, unsigned int size)
{
    return 0;
}


__attribute__((section(".utils"), used)) static void *utils_interface[4] = {
    (void *)open,
    (void *)close,
    (void *)write,
    (void *)read
    };
