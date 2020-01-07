#ifndef __ZMALLOC_H
#define __ZMALLOC_H
#include <sys/types.h>

void *zmalloc(size_t size);
void *zremalloc(void *ptr, size_t size);
void zfree(void *ptr);
void *zstrdup(const char *s);
size_t zmalloc_used_memory(void);
void zmalloc_enable_thread_safeness(void);

#endif
