#include <stdlib.h>
#include <stdbool.h>
#include "./quadimage.h"

quadimage_t* quadimage_init(aabb_t aabb) {
    quadimage_t* qt = (quadimage_t*) malloc(sizeof(quadimage_t));

    if ( qt == NULL ) {
        return NULL;
    }

    qt->aabb = aabb;
    qt->color = (color_t) { };
    qt->northEast = NULL;
    qt->northWest = NULL;
    qt->southWest = NULL;
    qt->southEast = NULL;

    return qt;
}

bool quadimage_split(quadimage_t* qt) {
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

    qt->northEast = quadimage_init(northEast);

    if ( qt->northEast == NULL ) {
        return false;
    }

    qt->northEast->color = qt->color;

    qt->southEast = quadimage_init(southEast);

    if ( qt->southEast == NULL ) {
        quadimage_clean(qt->northEast);
        return false;
    }

    qt->southEast->color = qt->color;

    qt->northWest = quadimage_init(northWest);

    if ( qt->northWest == NULL ) {
        quadimage_clean(qt->northEast);
        quadimage_clean(qt->southEast);
        return false;
    }

    qt->northWest->color = qt->color;


    qt->southWest = quadimage_init(southWest);

    if ( qt->southWest == NULL ) {
        quadimage_clean(qt->northEast);
        quadimage_clean(qt->southEast);
        quadimage_clean(qt->northWest);
        return false;
    }

    qt->southWest->color = qt->color;

    qt->fields.single = 1;

    return true;
}

bool quadimage_insert_point(quadimage_t* qt, unsigned int x, unsigned int y, color_t color) {
    if ( qt == NULL ) {
        return false;
    }

    if ( !aabb_contains(qt->aabb, x, y) ) {
        return false;
    }

    if ( aabb_is_single(qt->aabb) ) {
        qt->color = color;
        return true;
    }

    if ( qt->northEast == NULL ) {
        if ( !quadimage_split(qt) ) {
            return false;
        }
    }

    if ( quadimage_insert_point(qt->northEast, x, y, color) ) {
        qt->fields.dirty = 1;

        if ( !color_equals(color, qt->color) ) {
            qt->fields.single = 0;
        }

        return true;
    }
    else if ( quadimage_insert_point(qt->northWest, x, y, color) ) {
        qt->fields.dirty = 1;

        if ( !color_equals(color, qt->color) ) {
            qt->fields.single = 0;
        }

        return true;
    }
    else if ( quadimage_insert_point(qt->southEast, x, y, color) )  {
        qt->fields.dirty = 1;

        if ( !color_equals(color, qt->color) ) {
            qt->fields.single = 0;
        }

        return true;
    }
    else if ( quadimage_insert_point(qt->southWest, x, y, color) ) {
        qt->fields.dirty = 1;

        if ( !color_equals(color, qt->color) ) {
            qt->fields.single = 0;
        }

        return true;
    }

    return false;
}

void quadimage_fill_aabb(quadimage_t* qt, aabb_t aabb, color_t color) {
    if ( qt == NULL ) {
        return;
    }

    aabb_intersection_t inter = aabb_intersection(qt->aabb, aabb);

    switch ( inter ) {
        case NO_INTER:
            return;

        case FULL_INTER:
            qt->color = color;

            quadimage_clean(qt->northEast); qt->northEast = NULL;
            quadimage_clean(qt->southEast); qt->southEast = NULL;
            quadimage_clean(qt->northWest); qt->northWest = NULL;
            quadimage_clean(qt->southEast); qt->southEast = NULL;

            break;

        case PARTIAL_INTER:
            if ( qt->northEast == NULL ) {
                if ( !quadimage_split(qt) ) {
                    return;
                }
            }

            quadimage_fill_aabb(qt->northEast, aabb, color);
            quadimage_fill_aabb(qt->southEast, aabb, color);
            quadimage_fill_aabb(qt->northWest, aabb, color);
            quadimage_fill_aabb(qt->southEast, aabb, color);

            break;
    }

    return;
}

color_t quadimage_search(quadimage_t* qt, unsigned int x, unsigned int y) {
    if ( qt == NULL ) {
        return (color_t) { };
    }

    if ( qt->northEast != NULL ) {
        if ( aabb_contains(qt->northEast->aabb, x, y) ) { return quadimage_search(qt->northEast, x, y); }
        if ( aabb_contains(qt->northWest->aabb, x, y) ) { return quadimage_search(qt->northWest, x, y); }
        if ( aabb_contains(qt->southEast->aabb, x, y) ) { return quadimage_search(qt->southEast, x, y); }
        if ( aabb_contains(qt->southWest->aabb, x, y) ) { return quadimage_search(qt->southWest, x, y); }
    }
    else {
        return qt->color;
    }

    return (color_t) { };
}

void quadimage_clean(quadimage_t* qt) {
    if ( qt == NULL ) {
        return;
    }

    if ( qt->northEast != NULL ) {
        quadimage_clean(qt->northEast);
    }
    if ( qt->southEast != NULL ) {
        quadimage_clean(qt->southEast);
    }
    if ( qt->northWest != NULL ) {
        quadimage_clean(qt->northWest);
    }
    if ( qt->southWest != NULL ) {
        quadimage_clean(qt->southWest);
    }

    free(qt);
}
