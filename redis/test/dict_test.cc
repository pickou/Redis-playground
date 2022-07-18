#include <iostream>
#include "thirdparty/gtest/gtest.h"
#include "redis/src/dict.h"
#include "redis/src/hash.h"


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

