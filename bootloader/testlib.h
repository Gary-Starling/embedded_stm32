#ifndef _TEST_LIB_H
#define _TEST_LIB_H
int open(int d);
int close(int d);
int write(int d, void * buff, unsigned int size);
int read(int d, void *buf, unsigned int size);
int read(int d, char *byte);
#endif