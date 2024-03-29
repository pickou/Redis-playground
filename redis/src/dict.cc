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

static void _dictPanic(const char *fmt, ...);
static int _dictExpandIfNeeded(dict *d);
static void *_dictAlloc(size_t size);
static void _dictFree(dict *dt);
static void _dicthtReset(dictht *ht);
static void _dictReset(dict *dt);
static dictEntry *_dictEntryCreate();
static unsigned long _dictNextPower(unsigned long size);
static long _getHashKeyIndex(dict *dt, const void *key);
static void _dictEntryRelease(dictEntry *entry);


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

int _dictHtInit(dictht *dht) {
    // init table to void random pointer
    for (size_t i = 0; i < dht->size; ++i) {
        dht->table[i] = NULL;
    }
    return DICT_OK;
}

int _dictHtInit(dict *dt) {
    int isOk = _dictExpandIfNeeded(dt);
    if (!isOk) return isOk;
    isOk &= _dictHtInit(&dt->ht[0]);
    isOk &= _dictHtInit(&dt->ht[1]);
    return isOk;
}

int _dictInit(dict *dt, dictType *type, void *privdata) {
    _dictReset(dt);
    dt->type = type;
    dt->privdata = privdata;
    _dictHtInit(dt);
    return DICT_OK;
}

void _dictEntryRelease(dictht *dht) {
    for (size_t i = 0; i < dht->size; ++i) {
        dictEntry* p = dht->table[i];
        while (p != NULL) {
            dictEntry* tmp = p;
            p = p->next;
            _dictEntryRelease(tmp);
        }
    }
}

void _dictHtRelease(dictht *dht) {
    _dictEntryRelease(dht);
    zfree(dht->table);
}

void _dictHtRelease(dict *dt) {
    if (&dt->ht[0] != NULL) {
        _dictHtRelease(&dt->ht[0]);
    }
    if (&dt->ht[1] != NULL) {
        _dictHtRelease(&dt->ht[1]);
    }
}

void _privDataRelease(dict *dt) {
    if (dt->privdata != NULL) {
        zfree(dt->privdata);
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
    _privDataRelease(dt);
    _dictHtRelease(dt);
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
    index = index & ht.sizemask; // % operation
    // put the entry in the front
    entry->next = table[index];
    table[index] = entry;
    ht.used++;
    return dt;
}

// TODO: considering rehash
int dictExpand(dict *d, unsigned long size) {
    dictht n;
    unsigned long realsize = _dictNextPower(size);
    if(realsize == d->ht[0].size) return DICT_ERR;

    n.size = realsize;
    n.sizemask = realsize - 1;
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

dictEntry* getDictEntry(dict* dt, const void* key) {
    long idx = _getHashKeyIndex(dt, key);
    idx = idx & dt->ht[0].sizemask;
    dictEntry** table = dt->ht[0].table;
    if (table[idx] != NULL) {
        return table[idx];
    }
    idx = idx & dt->ht[1].sizemask;
    table = dt->ht[1].table;
    if (table[idx] != NULL) {
        return table[idx];
    }
    return NULL;
}

void* dictFind(dict *dt, const void* key) {
    return getDictEntry(dt, key)->value;
}
