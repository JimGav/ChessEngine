#ifndef LIST_H
#define LIST_H

#include "err_handle.h"

typedef struct {
    ListNode *head;
    int size;
    void (*destroy_func)(void *dt_ptr);
    int (*compare_func)(void *dt_ptr1, void *dt_ptr2);
} List;


typedef struct {
    void *dt_ptr;
    ListNode *next;
} ListNode;


List *list_create(List *list, int (*compare_func), void (*destroy_func));
status_t list_insert(List *list, void *dt_ptr);
status_t list_remove(List *list, void *dt_ptr);
status_t list_destroy(List *list);

#endif