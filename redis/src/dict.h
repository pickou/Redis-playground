#ifndef __DICT_H
#define __DICT_H
#include <sys/types.h>
#include <stdint.h>

typedef enum {
    DICT_ERR=0,
    DICT_OK
} DICT_STATUS;

typedef struct dictEntry {
    void *key; // key
    void *value; // value
    struct dictEntry *next; // pointer
} dictEntry;

typedef struct dictht {
    dictEntry **table; // hash table array
    unsigned long size; // table size
    unsigned long sizemask; // mask, keep it to size-1
    unsigned long used; // used count
} dictht;

typedef struct dictType {
    uint64_t (*hashFunction)(const void* key); // hash functions
    void *(*keyDup)(void *privdata, const void *key); // key dup
    void *(*valDup)(void *privdata, const void *obj); // value dup
    int (*keyCompare)(void *privdata, const void *key1, const void *key2); // key compare
    void (*keyDestructor)(void *privdata, void *key); // key destructor
    void (*valDestructor)(void *privdata, void *obj); // value destructor

    dictType() : hashFunction(NULL),
                 keyDup(NULL), valDup(NULL),
                 keyDestructor(NULL), valDestructor(NULL) { }
} dictType;

typedef struct dict {
    dictType *type; // type functions
    void *privdata; // private data
    dictht ht[2]; // hash table
    int rehashidx; // rehash index
} dict;

// const values
#define DICT_HT_INITIAL_SIZE 4

// dictType functions
#define dictHashKey(d, key) (d)->type->hashFunction(key)

// functions
dict *dictCreate(dictType *type, void *privdata); // create an empty dict
dict *dictCreate(); // create empty dict
void dictRelease(dict *dt); // release dict
dict *dictAdd(dict *dt, void *key, void *val); // add k/v
void *dictFind(dict *dt, const void *key); // use k to get v
dictEntry *dictGetRandomKey(dict *dt); // randomly get
int dictDelete(dict *dt, const void *key); // use k to delete k/v
int dictExpand(dict *d, unsigned long size); // expand dict
#endif
