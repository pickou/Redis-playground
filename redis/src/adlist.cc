#include <stdlib.h>
#include <stdio.h>
#include "redis/src/adlist.h"
#include "redis/src/zmalloc.h"

listNode *listFirst(list *list) {
    return list->head;
}

listNode *listTail(list *list) {
    return list->tail;
}

size_t listLength(list *list) {
    return list->len;
}

listNode *listPrevNode(listNode *node) {
    return node->prev;
}

listNode *listNextNode(listNode *node) {
    return node->next;
}

void *listNodeValue(listNode *node) {
    return node->value;
}

list *listCreate(void) {
    struct list *list = NULL;
    list = (struct list *)zmalloc(sizeof(struct list));
    if(list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;
    return list;
}

void listRelease(list *list) {
    if(list == NULL) return;
    // free each node
    listNode *curr, *next;
    size_t len = list->len;
    curr = next = list->head;
    while(len--) {
        next = curr->next;
        if(list->free) list->free(curr->value);
        zfree(curr);
        curr = next;
    }
    // free list
    safe_zfree(&list);
}

void safe_listRelease(list **list) {
    if(*list == NULL) return;
    listNode *curr, *next;
    size_t len = (*(list))->len;
    curr = next = (*(list))->head;
    while(len--) {
        next = curr->next;
        if((*(list))->free) (*(list))->free(curr->value);
        safe_zfree(&curr);
        curr = next;
    }
    safe_zfree(list);
}

list *listAddNodeHead(list *list, void *value) {
    listNode *node = (listNode *)zmalloc(sizeof(listNode));
    node->prev = NULL;
    node->next = list->head;
    if(list->head) {
        (list->head)->prev = node;
    } else {
        // head is tail
        list->tail = node;
    }
    node->value = value;
    list->head = node;
    list->len += 1;
    return list;
}

list *listAddNodeTail(list *list, void *value) {
    listNode *node = (listNode *)zmalloc(sizeof(listNode));
    node->next = NULL;
    node->prev = list->tail;
    if(list->tail) {
        (list->head)->next = node;
    } else {
        // hrad is tail
        list->head = node;
    }
    list->tail = node;
    node->value = value;
    list->len += 1;
    return list;
}

void listDelNode(list *list, listNode *node) {
    if(listLength(list) == 0) return;
    if(listLength(list) == 1) {
        if(list->head == node) {
            list->head = list->tail = NULL;
            if(list->free) list->free(node->value);
            zfree(node);
            list->len--;
            return;
        }
    }
    if(node == list->head) {
        list->head = node->next;
        (node->next)->prev = NULL;
    } else if(node == list->tail) {
        list->tail = node->prev;
        (node->prev)->next = NULL;
    } else {
        (node->prev)->next = node->next;
        (node->next)->prev = node->prev;
    }
    if(list->free) list->free(node->value);
    list->len--;
    zfree(node);
}

listNode *listSearchKey(list *list, void *key) {
    listIter *iter;
    listNode *node;

    iter = listGetIterator(list, FORWARD);
    while((node = listNext(iter)) != NULL) {
        if(list->match) {
            if(list->match(node->value, key)) {
                listReleaseIter(iter);
                return node;
            }
        } else {
            if(key == node->value) {
                listReleaseIter(iter);
                return node;
            }
        }
    }
    listReleaseIter(iter);
    return NULL;
}

listNode *listIndex(list *list, int index) {
    int i = 0;
    listNode *node = list->head;
    while(node) {
        if(i == index) return node;
        node = node->next;
        ++i;
    }
    return NULL;
}

list *listDup(list *orig) {
    list *copy = listCreate();
    copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;

    struct listIter *iter = listGetIterator(orig, FORWARD);
    while(iter->next) {
        listNode *node = listNext(iter);
        void *value;
        if(copy->dup) {
            value = copy->dup(node->value);
            if(value == NULL) {
                listReleaseIter(iter);
                listRelease(copy);
                return NULL;
            }
        } else {
            value = node->value;
        }
        if((copy = listAddNodeTail(copy, value)) == NULL) {
            listReleaseIter(iter);
            listRelease(copy);
            return NULL;
        }

    }
    return copy;
}

listIter *listGetIterator(list *list, int direction) {
   listIter *iter = (struct listIter*)zmalloc(sizeof(listIter*));
   iter->direction = direction;
   if(direction == FORWARD) {
       iter->next = list->head;
   } else {
       iter->next = list->tail;
   }
   return iter;
}

void listReleaseIter(listIter* iter) {
   zfree(iter);
}

listNode *listNext(listIter *iter) {
    if(!iter->next) return NULL;
    listNode *node = iter->next;
    if(iter->direction == FORWARD) {
        iter->next = (iter->next)->next;
    } else {
        iter->next = (iter->next)->prev;
    }
    return node;
}

void listRewind(list *list, listIter *iter) {
    iter->next = list->head;
    iter->direction = FORWARD;
}

void listRewindTail(list *list, listIter *iter) {
    iter->next = list->tail;
    iter->direction = BACKWARD;
}
