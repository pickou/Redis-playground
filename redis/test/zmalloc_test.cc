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
    EXPECT_EQ(get_used_memory(), 8 + 3);
    

    char *q = zmalloc(5);

    EXPECT_NE(q, nullptr);
    EXPECT_EQ(get_used_memory(), 11 + 8 + 5);
    zfree(p);
    EXPECT_EQ(get_used_memory(), 8 + 5);
    zfree(q);
    EXPECT_EQ(get_used_memory(), 0);
//    EXPECT_EQ(zmalloc(0), nullptr);
//    EXPECT_EQ(zmalloc(-1), nullptr);
}

TEST_F(ZmallocTest, Realloc) {
    // char p[] = "strafasf";
    
    char *p = zmalloc(4);
    char *q = zremalloc((void *)p, 10);
    EXPECT_NE(q, nullptr);
//    EXPECT_EQ(zremalloc(nullptr, 2), nullptr);
//    EXPECT_EQ(zremalloc(nullptr, -2), nullptr);
//    EXPECT_EQ(zremalloc(nullptr, 0), nullptr);
//    zfree(p); // cause double free
    zfree(q);
    
}

TEST_F(ZmallocTest, Free) {
    char *p = zmalloc(4);
    ASSERT_FALSE(p == NULL);
    zfree(p);
    EXPECT_EQ(get_used_memory(), 0);
}

TEST_F(ZmallocTest, Safezfree) {
    char *p = zmalloc(5);
    ASSERT_FALSE(p ==  NULL);
    EXPECT_EQ(get_used_memory(), 8 + 5);
    safe_zfree(&p);
    EXPECT_TRUE(p == NULL);
    EXPECT_EQ(get_used_memory(), 0);
}


