#include "sds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>


sds sdsnewlen(const void *init, size_t initlen) {
    struct sdshdr* sh;
    sh = zmalloc(sizeof(struct sdshdr)+initlen+1);
    if(sh==NULL) return NULL;
    sh->len = initlen;
    sh->free = 0;
    if(initlen) {
        if(init) {
            memcpy(sh->buf, init, initlen);
        } else {
            memset(sh->buf, 0, initlen);
        }
    }
    sh->buf[initlen] = '\0';
    return (char *)sh->buf;
}

sds sdsnew(const char *init) {
    size_t initlen = (init == NULL)?0:strlen(init);
    return sds sdsnewlen(init, initlen);

}

sds sdsempty() {
    return sdsnewlen("", 0);
}

size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-sizeof(struct sdshdr));
    return sh->len;
}

sds sdsdup(const sds s) {
    return sdsnew(s, sdslen(s));
}

void sdsfree(sds s) {
    if(s==NULL) return;
    zfree(s-sizeof(struct sdshdr));
}

size_t sdsavail(sds s) {
    struct sdshdr* sh = (void*)(s-sizeof(struct sdshdr));
    return sh->free;
}

static sds sdsMakeRooMFor(sds s, size_t addlen) {
    struct sdshdr *sh, *newsh;
    size_t free = sdsavail(s);

    size_t len, newlen;
    if(free>=addlen) return s;
    sh = (void*)(s-sizeof(struct sdshdr));
    newlen = (len+addlen)*2;
    newsh = zmalloc(sh, sizeof(struct sdshdr)+newlen+1);
    newsh->free = newlen - len;
    return newsh->buf;
}

sds sdscatlen(sds s, void *t, size_t len) {
    struct sdshdr *sh;
    size_t curlen = sdslen(s);

    s = sdsMakeRooMFor(s, len);
    if(s==NULL) return NULL;
    sh = (void*)(s-sizeof(struct sdshdr));
    memcpy(s+curlen, t, len);
    sh->len = curlen + len;
    sh->free = sh->free -len;
    s[curlen+len] = '\0';
    return s;
}

sds sdscat(sds s, char* t) {
    return sdscatlen(s, t, strlen(t));
}

sds sdscpylen(sds s, char *t, size_t len) {
    struct sdshdr *sh = (void *)(s-sizeof(struct sdshdr));
    size_t totallen = sh->free + sh->len;

    if(totallen<len) {
        s = sdsMakeRooMFor(s, len-sh->len);
        if(s==NULL) return NULL;
        sh = (void*)(s-sizeof(struct sdshdr));
        totallen = sh->free + sh->len;
    }

    memcpy(s, t, len);
    s[len] = '\0';
    sh->len = len;
    sh->free = totallen - len;
    return s;
}


