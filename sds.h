#ifndef __SDS_H
#define __SDS_H

#include <sys/types.h>


struct sdshdr {
    unsigned long _free;
    unsigned long _size;
    char buf[];
};

typedef char* sds;

sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);
sds sdsempty();
size_t sdslen(const sds s);
sds sdsdup(const sds s);
void sdsfree(sds s);
size_t sdsavail(sds s);
sds sdscatlen(sds s, void *t, size_t len);
sds sdscat(sds s, char *t);
sds sdscpylen(sds s, char *t, size_t len);
sds sdscpy(sds s, char *t);

sds sdstrim(sds s, const char *cset);
sds sdsrange(sds s, long start, long end);
void sdsupdatelen(sds s1, sds s2);
int sdscmp(sds s1, sds s2);
sds *sdssplitlen(char *s, int len, char *sep, int seplen, int *count);
void sdstolower(sds s);
void sdstoupper(sds s);

#endif
