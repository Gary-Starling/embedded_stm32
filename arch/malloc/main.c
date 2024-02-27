#include "malloc.h"

int main(void)
{
    char *mem1 = NULL;
    char *mem2 = NULL;
    char *mem3 = NULL;

    mem1 = malloc(10);
    mem2 = malloc(10);
   

    for (unsigned int i = 0; i < 10; i++)
    {
        mem1[i] = (char)i;
    }

    for (unsigned int i = 0; i < 10; i++)
    {
        mem2[i] = (char)i + 1;
    }

    free(mem1);
    free(mem2);
    mem3 = malloc(10);
    for (unsigned int i = 0; i < 10; i++)
    {
        mem3[i] = (char)i + 2;
    }
    free(mem3);

    while (1)
        ;
}