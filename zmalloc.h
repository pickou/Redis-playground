#ifndef __ZMALLOC_H
#define __ZMALLOC_H
#include <sys/types.h>

char *zmalloc(size_t size);
void *zremalloc(void *ptr, size_t size);
void zfree(void *ptr);
char *zstrdup(const char *s);
size_t zmalloc_used_memory(void);
void zmalloc_enable_thread_safeness(void);

#endif
