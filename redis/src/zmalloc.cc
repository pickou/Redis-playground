#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "redis/src/zmalloc.h"

#define PREFIX_SIZE sizeof(size_t)

#define increment_used_memory(_n) do { \
    used_memory += _n; \
} while(0)

#define decrement_used_memory(_n) do { \
    used_memory -= _n; \
} while(0)

static size_t used_memory = 0;

// stderr oom
void zmalloc_oom(size_t size) {
    fprintf(stderr, "zmalloc: Out of memeory trying to allocate %zu bytes.\n",
           size); 
    fflush(stderr);
    abort();
}

char *zmalloc(size_t size) {
    void *ptr = malloc(size + PREFIX_SIZE);

    // out of memory
    if(!ptr) zmalloc_oom(size);

    *((size_t *) ptr) = size;

    increment_used_memory(size + PREFIX_SIZE);
    
    return (char *)ptr + PREFIX_SIZE;
}


char *zremalloc(void *ptr, size_t size) {
    if(!ptr) return zmalloc(size);

    void *newptr;
    void *realptr = (char *)ptr - PREFIX_SIZE;
    
    size_t oldsize = *((size_t *)realptr);

    newptr = realloc(realptr, size + PREFIX_SIZE);
    if(!newptr) zmalloc_oom(size);
    
    *((size_t *) newptr) = size;

    decrement_used_memory(oldsize);
    increment_used_memory(size);
    
    return (char *)newptr + PREFIX_SIZE;
}

void zfree(void *ptr) {
    if(!ptr) return;
    void *realptr = (char *)ptr - PREFIX_SIZE;
    size_t oldsize = *((size_t *)realptr);
    decrement_used_memory(oldsize + PREFIX_SIZE);
    free(realptr);
}

size_t get_used_memory() {
    return used_memory;
}
