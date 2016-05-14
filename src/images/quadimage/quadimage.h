#include <stdint.h>
#include "../../aabb/aabb.h"
#include "../color/color.h"

typedef struct _quadimage {
    color_t color;
    aabb_t aabb;

    struct {
        uint8_t dirty  : 1;
        uint8_t single : 1;
    } fields;

    struct _quadimage* northEast;
    struct _quadimage* northWest;
    struct _quadimage* southEast;
    struct _quadimage* southWest;
} quadimage_t;

quadimage_t* quadimage_init(aabb_t aabb);
bool quadimage_insert_point(quadimage_t* qt, unsigned int x, unsigned int y, color_t data);
void quadimage_fill_aabb(quadimage_t* qt, aabb_t aabb, color_t color);
color_t quadimage_search(quadimage_t* qt, unsigned int x, unsigned int y);
void quadimage_clean(quadimage_t* qt);
