#pragma once

#include <stdbool.h>

typedef void (clean_fn_t)(void* obj);

typedef struct {
    void** items;
    unsigned int length;
    unsigned int alloced;
    unsigned int growth;
    clean_fn_t* clean_fn;
} list_t;

list_t* list_init(clean_fn_t* clean_fn);
bool list_grow(list_t** _list);
void list_add(list_t** _list, void* item);
void list_clean(list_t** _list);
