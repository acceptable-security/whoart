#include "aabb.h"

bool aabb_contains(aabb_t aabb, unsigned int x, unsigned int y) {
    return aabb.minX <= x && x < aabb.maxX &&
           aabb.minY <= y && y < aabb.maxY;
}

bool aabb_is_single(aabb_t aabb) {
    return (aabb.maxX - aabb.minX ) == 1 &&
           (aabb.maxY - aabb.minY ) == 1;
}

aabb_intersection_t aabb_intersection(aabb_t aabb1, aabb_t aabb2) {
    if ( ( aabb2.maxX < aabb1.minX || aabb2.minX > aabb1.minX) &&
         ( aabb2.maxY < aabb1.minY || aabb2.minY > aabb1.minY) ) {
        return NO_INTER;
    }

    if ( ( aabb2.maxX >= aabb1.minX && aabb2.minX <= aabb1.minX) &&
         ( aabb2.maxY >= aabb1.minY && aabb2.minY <= aabb1.minY) ) {
        return FULL_INTER;
    }

    return PARTIAL_INTER;
}
