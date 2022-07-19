#include <iostream>
#include "assert.h"
#include "thirdparty/gtest/gtest.h"
#include "redis/src/dict.h"
#include "redis/src/hash.h"
#include "redis/src/MurmurHash3.h"

class DictTest: public ::testing::Test {
    protected:
        DictTest() {

        }

        ~DictTest() override {}

        void SetUp() override {}

        void TearDown() override {}
};

TEST_F(DictTest, DictCreate) {
    dict *dict = dictCreate();
    EXPECT_NE(dict, NULL);
    dictRelease(dict);
}

TEST_F(DictTest, DictAdd) {
    dict *dict = dictCreate();
    unsigned char key[] = "redis dict";
    uint64_t val = 164;
    dictType dtype;
    dtype.hashFunction = simpleHash;
    dict->type = &dtype;
    dict = dictAdd(dict, key, val);
    dictRelease(dict);
}

TEST_F(DictTest, DictGet) {
    dict *dict = dictCreate();
    // set dtype
    dictType dtype;
    dtype.hashFunction = commonHash;
    dict->type = &dtype;
    // key1, val1
    unsigned char key1[] = "redis key1";
    uint64_t val1 = 1641;
    dict = dictAdd(dict, key1, val1);
    // key2, val2
    unsigned char key2[] = "redis key2";
    uint64_t val2 = 1642;
    dict = dictAdd(dict, key2, val2);
    // key3, val3
    unsigned char key3[] = "redis key3";
    uint64_t val3 = 1643;
    dict = dictAdd(dict, key3, val3);
    // dict get
    uint64_t val = dictFind(dict, key1);
    assert(val == 1643);
    dictRelease(dict);
}


