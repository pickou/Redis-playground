#include <iostream>
#include "thirdparty/gtest/gtest.h"
#include "redis/src/sds.h"


class SdsTest: public ::testing::Test {
    protected:
        SdsTest() {

        }

        ~SdsTest() override {}

        void SetUp() override {}

        void TearDown() override {}
};

TEST_F(SdsTest, Sdsnew) {
    char p[] = "adsfsfs";
    sds s = sdsnew(p);
    EXPECT_NE(s, nullptr);
    EXPECT_EQ(sdslen(s), 7);

    sdsfree(s);
}

TEST_F(SdsTest, Sdscat ) {
    // char p[] = "strafasf";
}
