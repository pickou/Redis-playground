#ifndef __ZMALLOC_H
#define __ZMALLOC_H
#include <sys/types.h>

char *zmalloc(size_t size);
char *zremalloc(void *ptr, size_t size);
void zfree(void *ptr);

#endif
