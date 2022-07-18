#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>

#include "redis/src/dict.h"
#include "redis/src/hash.h"
#include "redis/src/MurmurHash3.h"
#include "redis/src/zmalloc.h"

/* ----------- private functions ------------*/
static void _dictPanic(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "\n dict library panic: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n\n");
    va_end(ap);
}

static void *_dictAlloc(size_t size) {
    void *p = zmalloc(size);
    if(p == NULL) {
        _dictPanic("Out of memory");
    }
    return p;
}

static void _dictFree(dict *dt) {
    if(dt) {
        safe_zfree(&dt);
    }
}

static void _dicthtReset(dictht *ht) {
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

static void _dictReset(dict *dt) {
    dt->type = NULL;
    dt->privdata = NULL;
    dt->rehashidx = -1;
    // TODO: reset dictht
    _dicthtReset(&(dt->ht[0]));
    _dicthtReset(&(dt->ht[1]));
}

int _dictInit(dict *dt, dictType *type, void *privdata) {
    _dictReset(dt);
    dt->type = type;
    dt->privdata = privdata;
    return DICT_OK;
}

static dictEntry *_dictEntryCreate() {
    dictEntry *entry = (dictEntry *)zmalloc(sizeof(struct dictEntry));
    entry->key = NULL;
    entry->value = NULL;
    entry->next = NULL;
    return entry;
}

static void _dictEntryRelease(dictEntry *entry) {
    entry->key = NULL;
    entry->value = NULL;
    entry->next = NULL;
    safe_zfree(&entry);
}

static long _getHashKeyIndex(dict *dt, const void *key) {
    return dictHashKey(dt, key);
}

static int _dictExpandIfNeeded(dict *d) {
    // init
    if(d->ht[0].size == 0 && d->ht[1].size == 0) {
        return dictExpand(d, DICT_HT_INITIAL_SIZE);
    }
    // TODO: expand
    return 0;
}

// hash table capacity is power of 2
static unsigned long _dictNextPower(unsigned long size) {
    unsigned long i = DICT_HT_INITIAL_SIZE;

    if(size > LONG_MAX) return LONG_MAX + 1LU;
    while(1) {
        if(i >= size) return i;
        i *= 2;
    }
}
/* ------------ API functions -----------*/
dict *dictCreate(dictType *type, void *privdata) {
    dict *dt = _dictAlloc(sizeof(struct dict));
    _dictInit(dt, type, privdata);
    return dt;
}

dict *dictCreate() {
    dictType *type = NULL;
    char *privdata = NULL;
    return dictCreate(type, privdata);
}

void dictRelease(dict *dt) {
    // TODO: release privdata
    // TODO release ht
    _dictReset(dt);
    _dictFree(dt);
}

dictEntry *dictEntryCreate(void *key, void *value) {
    dictEntry *entry = _dictEntryCreate();
    entry->key = key;
    entry->value = value;
    return entry;
}

dict *dictAdd(dict *dt, void *key, void *val) {
    if(!dt) return dt;
    // TODO: consider rehash
    dictht ht = dt->ht[0];
    dictEntry **table = ht.table;
    dictEntry *entry = dictEntryCreate(key, val);
    long index = _getHashKeyIndex(dt, key);
    index = index & ht->sizemask; // % operation
    // put the entry in the front
    entry->next = table[index];
    table[index] = entry;
    ht.used++;
    return dt;
}

// TODO: set a simple hash function
void setSimpleHashFunc(dict *dt) {
    dt->type->hashFunction = simpleHash;
}

// TODO: considering rehash
int dictExpand(dict *d, unsigned long size) {
    dictht n;
    unsigned long realsize = _dictNextPower(size);
    if(realsize == d->ht[0].size) return DICT_ERR;

    n.size = realsize;
    m.sizemask = realsize - 1;
    n.table = zmalloc(realsize * sizeof(dictEntry*));
    n.used = 0;

    if(d->ht[0].table == NULL) {
        d->ht[0] = n;
        return DICT_OK;
    }

    d->ht[1] = n;
    d->rehashidx = 0;
    return DICT_OK;

}
