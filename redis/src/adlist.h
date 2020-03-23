#ifndef __ADLIST_H
#define __ADLIST_H
#include<sys/types.h>

enum DIRCTION {
    FORWARD = 0,
    BACKWARD
};

typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;
} listNode;

typedef struct listIter {
    listNode *next;
    int direction;
} listIter;

typedef struct list {
    listNode *head;
    listNode *tail;
    size_t len;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
} list;

listNode *listFirst(list *list);
listNode *listTail(list *list);
size_t listLength(list *list);
listNode *listPrevNode(listNode *node);
listNode *listNextNode(listNode *node);
void *listNodeValue(listNode *node);

void listSetDupMethod(list *list, void *func);
void listSetFreeMethod(list *list, void *func);
void listSetMatchMethod(list *list, void *func);

void *listGetDupMethod(list *list);
void *listGetFreeMethod(list *list);
void *listGetMAtchMethod(list *list);

list *listCreate(void);
void listRelease(list *list);
void safe_listRelease(list **list);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
void listDelNode(list *list, listNode *node);
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, int index);
list *listDup(list *orig);

// Iterator
listIter *listGetIterator(list *list, int direction);
listNode *listNext(listIter *iter);
void listReleaseIter(listIter *iter);
void listRewind(list *list, listIter *iter);
void listRewindTail(list *list, listIter *iter);

#endif
