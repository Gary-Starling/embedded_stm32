#ifndef _TEST_LIB_H
#define _TEST_LIB_H


static void **lib_interface = (void**)(0x08000400);

static inline int utils_open(int d)
{
    int (*do_open)(int) = (int (*)(int))(lib_interface[0]);
    return do_open(d);
}

static inline int utils_close(int d)
{
    int (*do_close)(int) = (int (*)(int))(lib_interface[1]);
    return do_close(d);
}


static inline int utils_write(int d, void *buf, int size)
{
    int (*do_write)(int, void *, int) = (int (*)(int, void *, int))(lib_interface[2]);
    return do_write(d, buf, size);
}

static inline int utils_read(int d, void *buf, int size)
{
    int (*do_read)(int, void *, int) = (int (*)(int, void *, int))(lib_interface[3]);
    return do_read(d, buf, size);
}


#endif