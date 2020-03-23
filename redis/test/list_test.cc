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

TEST_F(ListTest, ListAddNodeHeadTail) {
    list *list = listCreate();
    ASSERT_FALSE(list == NULL);
    char p[] = "12345";
    list = listAddNodeHead(list, p);
    EXPECT_EQ(1, listLength(list));
    EXPECT_EQ(listFirst(list), listTail(list));
    
    int a = 12345;
    list = listAddNodeTail(list, &a);
    EXPECT_EQ(2, listLength(list));
    EXPECT_FALSE(listTail(list) == NULL);
    EXPECT_EQ(a, *((int *)listNodeValue(listTail(list))));

    int *b = NULL;
    list = listAddNodeHead(list, b);
    EXPECT_EQ(3, listLength(list));
    safe_listRelease(&list);
}

TEST_F(ListTest, ListDelNode) {
    list *list = listCreate();
    int a[] = {1, 2, 4, 5};
    list = listAddNodeHead(list, a);
    char b[] = "12345";
    list = listAddNodeHead(list, b);
    listNode *head = listFirst(list);
    listNode *tail = listTail(list);

    ASSERT_EQ(2, listLength(list));
    listDelNode(list, head);
    EXPECT_EQ(1, listLength(list));
    EXPECT_EQ(tail, listTail(list));
    listDelNode(list, tail);
    EXPECT_EQ(0, listLength(list));
    EXPECT_EQ(listFirst(list), listTail(list));
    safe_listRelease(&list);
}

TEST_F(ListTest, ListSearchKey) {
    list *list = listCreate();
    int a = 10;
    list = listAddNodeHead(list, &a);
    listNode *node = listSearchKey(list, &a);
    ASSERT_FALSE(node == NULL);
    EXPECT_EQ(a, *((int *)listNodeValue(node)));

    int b = 20;
    listNode * nodeb = listSearchKey(list, &b);
    ASSERT_TRUE(nodeb == NULL);
    
    safe_listRelease(&list);
}

TEST_F(ListTest, ListIndex) {
    list *list = listCreate();
    listNode *node = listIndex(list, 0);

    ASSERT_TRUE(node == NULL);

    int a = 100;
    list = listAddNodeHead(list, &a);
    node = listIndex(list, 0);
    EXPECT_EQ(node, listFirst(list));

    int b = 200;
    list = listAddNodeTail(list, &b);
    node = listIndex(list, 1);
    EXPECT_EQ(node, listTail(list));

    safe_listRelease(&list);
}

TEST_F(ListTest, ListIterGetRelease) {
    list *list = listCreate();
    int a = 10;
    list = listAddNodeHead(list, &a);
    int b = 20;
    list = listAddNodeHead(list, &b);
    listIter *iter = listGetIterator(list, FORWARD);
    while(iter->next) {
        listNode *node = listNext(iter);
        EXPECT_FALSE(node == NULL);
    }
    listReleaseIter(iter);
    listRelease(list);
    
}

TEST_F(ListTest, ListDup) {
    list *list = listCreate();
    int a = 10;
    list = listAddNodeHead(list, &a);
    int b = 20;
    list = listAddNodeHead(list, &b);
    int c = 30;
    list = listAddNodeHead(list, &c);
    struct list *copy = listDup(list);
    /*
    listNode *headc = listFirst(copy);
    listNode *head = listFirst(list);
    ASSERT_FALSE(headc == head);
    ASSERT_EQ(3, listLength(copy));
    EXPECT_EQ(c, *((int *)listNodeValue(headc)));
    */
    safe_listRelease(&copy);
    safe_listRelease(&list);
}

TEST_F(ListTest, ListFree) {

}

TEST_F(ListTest, ListMatch) {

}
