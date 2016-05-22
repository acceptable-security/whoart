#include <stdlib.h>
#include <string.h>
#include "util.h"

list_t* list_init(clean_fn_t* clean_fn) {
    list_t* list = (list_t*) malloc(sizeof(list_t));

    if ( list == NULL ) {
        return NULL;
    }

    list->clean_fn = clean_fn;
    list->length = 0;
    list->alloced = 8;
    list->growth = 8;
    list->items = malloc(sizeof(void*) * list->alloced);

    if ( list->items == NULL ) {
        free(list);
        return NULL;
    }

    memset(list->items, 0, sizeof(void*) * list->alloced);

    return list;
}

bool list_grow(list_t** _list) {
    list_t* list = *_list;

    if ( list == NULL ) {
        return false;
    }

    list->alloced += list->growth;
    void** tmp = realloc(list->items, list->alloced * sizeof(void*));

    if ( tmp == NULL ) {
        return false;
    }

    list->items = tmp;
    return true;
}

void list_add(list_t** _list, void* item) {
    list_t* list = *_list;

    if ( list == NULL ) {
        return;
    }

    if ( list->length >= list->alloced ) {
        if ( !list_grow(_list) ) {
            list_clean(_list);
            return;
        }
    }

    list->items[list->length++] = item;
}

void list_clean(list_t** _list) {
    list_t* list = *_list;

    if ( list == NULL ) {
        return;
    }

    if ( list->items != NULL ) {
        if ( list->clean_fn != NULL ) {
            for ( int i = 0; i < list->length; i++ ) {
                list->clean_fn(list->items[i]);
            }
        }

        free(list->items);
    }

    free(list);
}
