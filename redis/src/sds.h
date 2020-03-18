#ifndef __SDS_H
#define __SDS_H
#include <sys/types.h>

typedef char *sds;

struct sdshdr {
    long len;
    long free;
    char buf[];
};

sds sdsnew(const char *init);
sds sdsempty();
void sdsfree(sds s);
int sdslen(sds s);
size_t sdsavail(sds s);
void sdsclear(sds s);
sds sdscat(sds s, char *t);
sds sdscpy(sds s, char *t);
int sdscmp(sds s1, sds s2);

sds sdsnewlen(const void *init, size_t initlen);
sds sdsdup(const sds s);
sds sdscpy(sds s, char *t, size_t len);
sds sdstrim(sds s, const char *cset);
sds sdsrange(sds s, long start, long end);
void sdstolower(sds s);
void sdstoupper(sds s);

#endif
