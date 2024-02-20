#include "user_malloc.h"

extern unsigned int _start_heap;

struct user_malloc_block
{
   unsigned int sign;
   unsigned int size;
};

void* malloc(unsigned int size)
{
    static unsigned int *end_heap = 0;
    struct user_malloc_block *blk;
    char *ret = NULL;
    
    if(end_heap == 0) end_heap = &_start_heap;

    //alignment word(32 bit)
    if(((size >> 2) << 2) != size)
    size = ((size >> 2) + 1) << 2;

    blk = (struct user_malloc_block *)&_start_heap;

    //find free block in heap
    while ((unsigned int*)blk < end_heap)
    {
        if((blk->sign == SIGN_FRED) && (blk->size)>= size)
        {
            blk->sign = SIGN_USED;
            ret = ((char*)blk) + sizeof(struct user_malloc_block);
            return ret;
        }
        blk = (struct user_malloc_block *)((char *)blk) + sizeof(struct user_malloc_block) + blk->size;
    }
    
    //or alloc here
    blk = (struct user_malloc_block *)end_heap;
    blk->sign = SIGN_USED;
    blk->size = size;
    ret = ((char*)blk) + sizeof(struct user_malloc_block);
    end_heap = (unsigned int *)(ret + size);
    return ret;
}


void free(void *p)
{
    struct user_malloc_block *blk = (struct user_malloc_block *)(((char *)(p) - sizeof(struct user_malloc_block)));

    if(!p) return;
    if(blk->sign != SIGN_USED) return;
    blk->sign = SIGN_FRED;
}

