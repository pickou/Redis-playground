#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "redis/src/zmalloc.h"

// stderr oom
void zmalloc_oom(size_t size) {
    fprintf(stderr, "zmalloc: Out of memeory trying to allocate %zu bytes.\n",
           size); 
    fflush(stderr);
    abort();
}

char *zmalloc(size_t size) {
    void *ptr = malloc(size);
    // out of memory
    if(!ptr) zmalloc_oom(size);

    *((size_t *) ptr) = size;

    return (char *)ptr;
}


char *zremalloc(void *ptr, size_t size) {
    if(!ptr) return zmalloc(size);

    void *newptr;

    newptr = realloc(ptr, size);
    
    if(!newptr) zmalloc_oom(size);
    
    *((size_t *) newptr) = size;

    return (char *)newptr;

}

void zfree(void *ptr) {
    if(!ptr) return;
    free(ptr);
}
