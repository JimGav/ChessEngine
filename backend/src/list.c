#include <stdlib.h>
#include <assert.h>
#include "list.h"


List *list_create(int (*compare_func)(void*,void*), void (*destroy_func)(void*)){

    // if (compare_func == NULL)
    //     PSTAT(STAT_NULLPTR, "list_create compare_func");

    List *list = calloc(1, sizeof(List));
    if (list == NULL)
        PSTAT(STAT_ERR, "list_create calloc")

    list->compare_func = compare_func;
    list->destroy_func = destroy_func;
    list->size = 0;
    return list;
}


status_t list_insert(List *list, void *dt_ptr){
    if (list == NULL || dt_ptr == NULL)
        return STAT_NULLPTR;
    
    ListNode *node = calloc(1, sizeof(ListNode));
    assert(node != NULL);
    node->dt_ptr = dt_ptr;

    if (list->head == NULL)
        list->head = node;
    else{
        node->next = list->head;
        list->head = node;
    }

    list->size++;
    return STAT_SUCCESS;
}


status_t list_remove(List *list, void *dt_ptr){
    if (list == NULL || dt_ptr == NULL)
        return STAT_NULLPTR;    
    ListNode *curr = list->head;
    ListNode *prev = NULL;
    while (curr && list->compare_func(curr->dt_ptr, dt_ptr) != 0){
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL)   // not found
        return STAT_NOTFOUND;
    
    // found
    if (prev)
        prev->next = curr->next;
    else
        list->head = curr->next;
    if (list->destroy_func)
        list->destroy_func(curr);
    list->size--;
    return STAT_SUCCESS;
}


status_t list_destroy(List *list){
    ListNode *curr = list->head;
    ListNode *next = NULL;
    while (curr){
        next = curr->next;
        list->destroy_func(curr);
        curr = next;
    }
    free(list);
    return STAT_SUCCESS;
}