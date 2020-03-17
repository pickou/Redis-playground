#include <iostream>
#include "thirdparty/gtest/gtest.h"
#include "redis/src/zmalloc.h"


class ZmallocTest: public ::testing::Test {
    protected:
        ZmallocTest() {

        }

        ~ZmallocTest() override {}

        void SetUp() override {}

        void TearDown() override {}
};

TEST_F(ZmallocTest, Malloc) {
    char *p = zmalloc(3);
    EXPECT_NE(p, nullptr);
    zfree(p);
//    EXPECT_EQ(zmalloc(0), nullptr);
//    EXPECT_EQ(zmalloc(-1), nullptr);
}

TEST_F(ZmallocTest, Realloc) {
    // char p[] = "strafasf";
    int *p = (int *)malloc(8);
    char *q = zremalloc((void *)p, 10);
    EXPECT_NE(q, nullptr);
//    EXPECT_EQ(zremalloc(nullptr, 2), nullptr);
//    EXPECT_EQ(zremalloc(nullptr, -2), nullptr);
//    EXPECT_EQ(zremalloc(nullptr, 0), nullptr);
    free(p);
    zfree(q);
}
