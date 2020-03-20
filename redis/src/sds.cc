#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "redis/src/sds.h"
#include "redis/src/zmalloc.h"

static void sdsOomAbort(void) {
    fprintf(stderr, "SDS: Out of memory.\n");
    abort();
}

sds sdsnewlen(const void *init, size_t initlen) {
   struct sdshdr *sh;

   sh = (struct sdshdr *) zmalloc(sizeof(struct sdshdr) + initlen + 1);

   if(!sh) sdsOomAbort();

   sh->len = initlen;
   sh->free = 0;

   if(initlen) {
       if(init) memcpy(sh->buf, init, initlen);
       else memset(sh->buf, 0, initlen);
   }

   sh->buf[initlen] = '\0';
   return (char *)sh->buf;
}

sds sdsnew(const char *init) {
    size_t initlen = (init == NULL) ? 0: strlen(init);
    return sdsnewlen(init, initlen);
}

sds sdsempty(void) {
    return sdsnewlen("", 0);
}

void sdsfree(sds s) {
    if(!s) return;
    void *p = (void *)(s - sizeof(struct sdshdr));
    // safe_zfree(&p);
    zfree(p);
}

int sdslen(sds s) {
    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    return sh->len;
}

size_t sdsavail(sds s) {
    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    return sh->free;
}

void sdsclear(sds s) {
    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    sh->free += sh->len;
    sh->len = 0;
}

static sds sdsMakeRoomFor(sds s, size_t addlen) {
    if(sdsavail(s) >= addlen) return s;

    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
   
    size_t len = sh->len;
    size_t newlen = (len + addlen) * 2;
    struct sdshdr *newsh = (struct sdshdr *)
        zremalloc(sh, sizeof(struct sdshdr) + newlen + 1);
    
    if(!newsh) sdsOomAbort();

    newsh->free = newlen - newsh->len;
    return newsh->buf;
}

sds sdscatlen(sds s, void *t, size_t len) {
    struct sdshdr *sh;
    size_t curlen = sdslen(s);
    
    s = sdsMakeRoomFor(s, len);

    if(s == NULL) return NULL;

    sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    memcpy(s + curlen, t, len);
    sh->len = curlen + len;
    sh->free -= len;
    s[sh->len] = '\0';
    return s;
}

sds sdscat(sds s, char *t) {
    return sdscatlen(s, t, strlen(t));  
}

sds sdscpylen(sds s, char *t, size_t len) {
    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    size_t totlen = sh->free + sh->len;
    
    if(totlen < len) {
        s = sdsMakeRoomFor(s, len - sh->len);
        if(!s) sdsOomAbort();
        // the addr of s has changed.
        sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
        totlen = sh->free + sh->len;
    }

    memcpy(s, t, len);
    sh->buf[len] = '\0';
    sh->len = len;
    sh->free = totlen - len;
    return s;
}

sds sdscpy(sds s, char *t) {
    return sdscpylen(s, t, strlen(t));
}

int sdscmp(sds s1, sds s2) {
    size_t len1, len2, minlen;
    len1 = sdslen(s1);
    len2 = sdslen(s2);

    minlen = (len1 >= len2) ? len2:len1;
    int cmp = memcmp(s1, s2, minlen);
    
    if(cmp == 0) return len1 - len2;
    return cmp;
}

sds sdsdup(const sds s) {
    return sdsnewlen(s, sdslen(s)); 
}

sds sdstrim(sds s, const char *cset) {
    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    char *start, *sp, *end, *ep;

    start = sp = s;
    end = ep = s + sdslen(s);

    while(sp <= end && strchr(cset, *sp)) sp++;
    while(ep > start && strchr(cset, *ep)) ep--;

    size_t len = (sp > ep) ? 0:(ep-sp+1);

    memmove(sh->buf, sp, len);
    sh->free += sh->len - len;
    sh->len = len;
    sh->buf[len] = '\0';
    return s;
}

sds sdsrange(sds s, long start, long end) {
    struct sdshdr *sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
    size_t newlen, len = sdslen(s);

    if(len == 0) return s;

    // support back index
    if(start < 0) {
        start += len;
        if(start < 0) start = 0;
    }
    if(end < 0) {
        end += len;
        if(end < 0) end = 0;
    }

    newlen = (start > end) ? 0:(end-start+1);
    memmove(sh->buf, sh->buf+start, newlen);
    sh->free += sh->len - newlen;
    sh->len = newlen;
    sh->buf[newlen] = '\0';
    return s;

}

void sdstolower(sds s) {
   size_t len = sdslen(s);
   for(size_t i=0; i<len; ++i) {
       s[i] = tolower(s[i]);
   }
}

void sdstoupper(sds s) {
   size_t len = sdslen(s);
   for(size_t i=0; i<len; ++i) {
       s[i] = toupper(s[i]);
    }
}
