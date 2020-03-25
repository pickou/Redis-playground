#include <iostream>
#include "thirdparty/gtest/gtest.h"
#include "redis/src/zmalloc.h"
#include "redis/src/adlist.h"


class ListTest: public ::testing::Test {
    protected:
        ListTest() {

        }

        ~ListTest() override {}

        void SetUp() override {}

        void TearDown() override {}
};

void *intArrDup(void *ptr) {
    int *arr_orig = (int *)ptr;
    size_t size = zsizeof(ptr);
    int *arr = (int *)zmalloc(size);
    size_t len = size/sizeof(int);
    // printf("sizeof arr: %zu, len: %zu\n", size, len);
    for(size_t i=0; i<len; ++i) {
        *(arr+i) = *(arr_orig+i);
        // do not operate the original pointer
        // *arr++ = *arr_orig++; // this can cause terrible result
    }
    return arr;
}

int intMatch(void *ptr, void *key) {
    int ptr_val = *((int *)ptr);
    int key_val = *((int *)key);
    return ptr_val == key_val ? 1: 0;
}

int intArrMatch(void *ptr, void *key) {
    int *ptr_int = (int *)ptr;
    int *key_int = (int *)key;
    size_t size_ptr = zsizeof(ptr_int);
    size_t size_key = zsizeof(key_int);
    if(size_ptr != size_key) return 0;
    for(size_t i=0; i<size_ptr; ++i) {
        if(*(ptr_int + i) != *(key_int + i)) return 0;
    }
    return 1;
}

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

    // add array to a node
    int *aa = (int *)zmalloc(16);
    for(int i=0; i<4; ++i) {
        *(aa+i) = i;
    }
    list = listAddNodeTail(list, aa);
    listNode *node = listTail(list);
    int *aa_ptr = (int *) listNodeValue(node);
    ASSERT_EQ(16, zsizeof(aa_ptr));
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(aa[i], *(aa_ptr+i));
    }
    safe_zfree(&aa);
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
    listNode *headc = listFirst(copy);
    listNode *head = listFirst(list);
    ASSERT_FALSE(headc == head);
    ASSERT_EQ(3, listLength(copy));
    EXPECT_EQ(c, *((int *)listNodeValue(headc)));
    safe_listRelease(&copy);
    safe_listRelease(&list);

    // add int array && dup list
    list = listCreate();
    int *aa = (int *)zmalloc(16);
    for(int i=0; i<4; ++i) {
        *(aa + i) = i + 1;
    }
    list = listAddNodeHead(list, aa);
    int *bb = (int *)zmalloc(16);
    for(int i=0; i<4; ++i) {
        *(bb + i) = 10 * (i + 1);
    }
    list = listAddNodeTail(list, bb);
    list->dup = intArrDup;
    list->free = zfree;
    copy = listDup(list);
    head = listFirst(list);
    headc = listFirst(copy);
    ASSERT_FALSE(head == NULL);
    
    int *arr = (int *)listNodeValue(head);
    int *arrc = (int *)listNodeValue(headc);
    
    for(int i=0; i<4; ++i) {
        EXPECT_EQ(aa[i], *(arr+i));
        EXPECT_EQ(aa[i], *(arrc+i));
    }
       
    safe_listRelease(&copy);
    safe_listRelease(&list);
}

TEST_F(ListTest, ListFree) {
   int *arr = (int *)zmalloc(8);
   *(arr) = 1;
   *(arr+1) = 2;
   list *list = listCreate();
   list = listAddNodeHead(list, arr);
   list->free = zfree;
   ASSERT_FALSE(list->free == NULL);
   safe_listRelease(&list);
}

TEST_F(ListTest, ListMatch) {
    int a = 10;
    int b = 1;
    int c = 55;
    list *list = listCreate();
    list = listAddNodeHead(list, &a);
    list = listAddNodeHead(list, &b);
    list = listAddNodeHead(list, &c);
    list->match = intMatch;
    listNode* node = listSearchKey(list, &a);
    ASSERT_EQ(node, listTail(list));
    EXPECT_EQ(a, *((int *)listNodeValue(node)));
    safe_listRelease(&list);
}

TEST_F(ListTest, ListArrMatch) {
    list *list = listCreate();
    list->match = intArrMatch;
    list->free = zfree;
    int *aa = (int *)zmalloc(sizeof(int)*4);
    for(int i=0; i<4; ++i) *(aa + i) = i + 1;
    int *bb = (int *)zmalloc(sizeof(int)*5);
    for(int i=0; i<5; ++i) *(bb + i) = i + 1;
    int *cc = (int *)zmalloc(sizeof(int)*6);
    for(int i=0; i<6; ++i) *(cc + i) = i + 1;
    list = listAddNodeHead(list, aa);
    list = listAddNodeHead(list, bb);

    listNode *node = listSearchKey(list, aa);
    ASSERT_EQ(node, listTail(list));
    
    node = listSearchKey(list, cc);
    ASSERT_TRUE(node == NULL);
    safe_zfree(&cc);
    safe_listRelease(&list);
}
