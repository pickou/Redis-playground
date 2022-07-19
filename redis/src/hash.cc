#include <sys/types.h>
#include <string.h>
#include "redis/src/hash.h"

// TODO: use redis siphash && murmurhash
uint64_t simpleHash(const void *key) {
  return 1;
}

uint64_t commonHash(const void *key) {
    uint64_t out[2];
    const int len = strlen((const char *)key);
    MurmurHash3_x64_128(key, len, SEED, out);
    return out[1];
}
