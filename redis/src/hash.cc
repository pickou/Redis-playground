#include <sys/types.h>
#include "redis/src/hash.h"

// TODO: use redis siphash && murmurhash
uint64_t simpleHash(const void *key) {
  return 1;
}