#ifndef __HASH_H_
#define __HASH_H_
#include <sys/types.h>
#include <stdint.h>
#include "redis/src/MurmurHash3.h"

static uint32_t SEED = 0x1234ABCD;
uint64_t simpleHash(const void *key);
uint64_t commonHash(const void *key);

#endif
