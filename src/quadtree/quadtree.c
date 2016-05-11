#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"

node_t* node_init(unsigned int x, unsigned int y, void* data) {
    node_t* node = (node_t*) malloc(sizeof(node_t));

    if ( node == NULL ) {
        return NULL;
    }

    node->x = x;
    node->y = y;
    node->data = data;

    return node;
}

void node_clean(node_t* node) {
    if ( node == NULL ) {
        return;
    }

    free(node);
}

quadtree_t* quadtree_init(aabb_t aabb, unsigned int capacity) {
    quadtree_t* qt = (quadtree_t*) malloc(sizeof(quadtree_t));

    if ( qt == NULL ) {
        return NULL;
    }

    qt->data = malloc(sizeof(void*) * capacity);

    if ( qt->data == NULL ) {
        free(qt);
        return NULL;
    }


    qt->aabb = aabb;
    qt->capacity = capacity;
    qt->length = 0;

    qt->northEast = NULL;
    qt->northWest = NULL;
    qt->southWest = NULL;
    qt->southEast = NULL;

    return qt;
}

bool quadtree_split(quadtree_t* qt) {
    if ( qt == NULL ) {
        return false;
    }

    if ( qt->northEast != NULL ) {
        return false;
    }

    aabb_t southWest = (aabb_t) {
        .minX = qt->aabb.minX,
        .maxX = (qt->aabb.minX + qt->aabb.maxX) / 2,
        .minY = qt->aabb.minY,
        .maxY = (qt->aabb.minY + qt->aabb.maxY) / 2,
    };

    aabb_t southEast = (aabb_t) {
        .minX = (qt->aabb.minX + qt->aabb.maxX) / 2,
        .maxX = qt->aabb.maxX,
        .minY = qt->aabb.minY,
        .maxY = (qt->aabb.minY + qt->aabb.maxY) / 2,
    };

    aabb_t northEast = (aabb_t) {
        .minX = (qt->aabb.minX + qt->aabb.maxX) / 2,
        .maxX = qt->aabb.maxX,
        .minY = (qt->aabb.minY + qt->aabb.maxY) / 2,
        .maxY = qt->aabb.maxY,
    };

    aabb_t northWest = (aabb_t) {
        .minX = qt->aabb.minX,
        .maxX = (qt->aabb.minX + qt->aabb.maxX) / 2,
        .minY = (qt->aabb.minY + qt->aabb.maxY) / 2,
        .maxY = qt->aabb.maxY,
    };

    qt->northEast = quadtree_init(northEast, qt->capacity);

    if ( qt->northEast == NULL ) {
        return false;
    }

    qt->southEast = quadtree_init(southEast, qt->capacity);

    if ( qt->southEast == NULL ) {
        quadtree_clean(qt->northEast);
        return false;
    }

    qt->northWest = quadtree_init(northWest, qt->capacity);

    if ( qt->northWest == NULL ) {
        quadtree_clean(qt->northEast);
        quadtree_clean(qt->southEast);
        return false;
    }

    qt->southWest = quadtree_init(southWest, qt->capacity);

    if ( qt->southWest == NULL ) {
        quadtree_clean(qt->northEast);
        quadtree_clean(qt->southEast);
        quadtree_clean(qt->northWest);
        return false;
    }

    return true;
}

bool quadtree_insert(quadtree_t* qt, unsigned int x, unsigned int y, void* data) {
    if ( qt == NULL ) {
        return false;
    }

    if ( !aabb_contains(qt->aabb, x, y) ) {
        return false;
    }

    if ( qt->length < qt->capacity ) {
        qt->data[qt->length++] = node_init(x, y, data);
        return true;
    }

    if ( qt->northEast == NULL ) {
        if ( !quadtree_split(qt) ) {
            return false;
        }
    }

    if ( quadtree_insert(qt->northEast, x, y, data) ) { return true; }
    if ( quadtree_insert(qt->northWest, x, y, data) ) { return true; }
    if ( quadtree_insert(qt->southEast, x, y, data) ) { return true; }
    if ( quadtree_insert(qt->southWest, x, y, data) ) { return true; }

    return false;
}

void* quadtree_search(quadtree_t* qt, unsigned int x, unsigned int y) {
    if ( qt == NULL ) {
        return NULL;
    }

    if ( qt->length == 0 ) {
        return NULL;
    }

    if ( !aabb_contains(qt->aabb, x, y) ) {
        return NULL;
    }

    for ( int i = 0; i < qt->length; i++ ) {
        if ( qt->data[i]->x == x && qt->data[i]->y == y ) {
            return qt->data[i]->data;
        }
    }

    if ( qt->length >= qt->capacity && qt->northEast != NULL ) {
        void* out = quadtree_search(qt->northEast, x, y);

        if ( out != NULL ) {
            return out;
        }

        out = quadtree_search(qt->northWest, x, y);

        if ( out != NULL ) {
            return out;
        }

        out = quadtree_search(qt->southWest, x, y);

        if ( out != NULL ) {
            return out;
        }

        out = quadtree_search(qt->southEast, x, y);

        if ( out != NULL ) {
            return out;
        }
    }

    return NULL;
}

void _indent(int amt) {
    for ( int i = 0; i < amt; i++ ) printf("+- ");
}

void quadtree_debug(quadtree_t* qt, int indent) {
    _indent(indent); printf("Quad Tree w/ AABB (%d, %d) (%d, %d)\n", qt->aabb.minX, qt->aabb.minY, qt->aabb.maxX, qt->aabb.maxY);
    _indent(indent); printf("Length: %d\n", qt->length);

    if ( qt->northEast != NULL ) {
        quadtree_debug(qt->northEast, indent + 1);
        quadtree_debug(qt->northWest, indent + 1);
        quadtree_debug(qt->southEast, indent + 1);
        quadtree_debug(qt->southWest, indent + 1);
    }
}

void quadtree_clean(quadtree_t* qt) {
    if ( qt == NULL ) {
        return;
    }

    if ( qt->northEast != NULL ) {
        quadtree_clean(qt->northEast);
    }
    if ( qt->southEast != NULL ) {
        quadtree_clean(qt->southEast);
    }
    if ( qt->northWest != NULL ) {
        quadtree_clean(qt->northWest);
    }
    if ( qt->southWest != NULL ) {
        quadtree_clean(qt->southWest);
    }

    if ( qt->data != NULL ) {
        for ( int i = 0; i < qt->length; i++ ) {
            node_clean(qt->data[i]);
        }

        free(qt->data);
    }

    free(qt);
}
