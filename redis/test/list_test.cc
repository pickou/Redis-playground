#include <iostream>
#include "thirdparty/gtest/gtest.h"
#include "redis/src/adlist.h"


class ListTest: public ::testing::Test {
    protected:
        ListTest() {

        }

        ~ListTest() override {}

        void SetUp() override {}

        void TearDown() override {}
};

TEST_F(ListTest, ListCreate) {
    struct list *list = listCreate();
    ASSERT_TRUE(list != NULL);
    safe_listRelease(&list);
    EXPECT_TRUE(list == NULL);
}

