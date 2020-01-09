#include "zmalloc.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define PREFIX_SIZE sizeof(size_t)

#define increment_used_memory(_n) do { \
        used_memory += _n; \
    } while(0)

#define decrement_used_memory(_n) do { \
        used_memory -= _n; \
    } while(0)
/*
 * for now we don't consider the thread safe condition
 * 2020-01-08
 */
static size_t used_memory = 0;

static void zmalloc_oom(size_t size) {
    fprintf(stderr, "zmalloc : Out of memory trying to allocate %zu bytes\n", 
            size);
    fflush(stderr);
    abort();
}

char *zmalloc(size_t size) {
    void *ptr = malloc(size+PREFIX_SIZE);
    
    if(!ptr) zmalloc_oom(size);

    *((size_t*)ptr) = size;
    increment_used_memory(size+PREFIX_SIZE);
    return (char*)ptr+PREFIX_SIZE;
}

void *zremalloc(void *ptr, size_t size) {
    void *realptr;
    size_t old_size;
    void *newptr;

    if(ptr == NULL) return zmalloc(size);

    realptr = (char*)ptr - PREFIX_SIZE;
    old_size = *((size_t*)realptr);
    newptr = realloc(realptr, size+PREFIX_SIZE);
    if(!newptr) zmalloc_oom(size);

    *((size_t*)newptr) = size;
    decrement_used_memory(old_size);
    increment_used_memory(size);
    return (char*)newptr+PREFIX_SIZE;

}

void zfree(void *ptr) {
    void *realptr;
    size_t old_size;
    
    if(ptr == NULL) return;

    realptr = (char *)ptr - PREFIX_SIZE;
    old_size = *((size_t*)realptr);
    decrement_used_memory(old_size+PREFIX_SIZE);
    free(realptr);
}

char *zstrdup(const char *s) {
    size_t l = strlen(s)+1;
    char *p = zmalloc(l);

    memcpy(p,s,l);
    return p;
}

size_t zmalloc_used_memory(void) {
    size_t um;
    um = used_memory;
    return um;
}


void zmalloc_enable_thread_safeness(void) {

}

