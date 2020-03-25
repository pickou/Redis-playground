#ifndef __ZMALLOC_H
#define __ZMALLOC_H
#include <sys/types.h>

#ifdef DEBUG
void safe_zfree(void **p) {
    zfree(*p);
    *p = NULL;
}
#else 
#define safe_zfree(p) do { \
    void **_p = p;         \
    zfree(*_p);            \
    *_p = NULL;            \
} while(0)
#endif

void *zmalloc(size_t size);
void *zremalloc(void *ptr, size_t size);
void zfree(void *ptr);
size_t get_used_memory();
size_t zsizeof(void *ptr);

#endif
