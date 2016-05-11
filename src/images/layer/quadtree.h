#include <stdbool.h>
#include "../../aabb/aabb.h"

typedef struct {
    unsigned int x;
    unsigned int y;
    void* data;
} node_t;

typedef struct _quadtree {
    unsigned int capacity;
    unsigned int length;
    node_t** data;

    aabb_t aabb;

    struct _quadtree* northEast;
    struct _quadtree* northWest;
    struct _quadtree* southEast;
    struct _quadtree* southWest;
} quadtree_t;

quadtree_t* quadtree_init(aabb_t aabb, unsigned int capacity);
bool quadtree_split(quadtree_t* qt);
bool quadtree_insert(quadtree_t* qt, unsigned int x, unsigned int y, void* data);
void* quadtree_search(quadtree_t* qt, unsigned int x, unsigned int y);
void quadtree_debug(quadtree_t* qt, int indent);
void quadtree_clean(quadtree_t* qt);
