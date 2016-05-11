#include <stdbool.h>

typedef struct {
    unsigned int minX;
    unsigned int maxX;
    unsigned int minY;
    unsigned int maxY;
} aabb_t;

typedef enum {
    NO_INTER,
    PARTIAL_INTER,
    FULL_INTER
} aabb_intersection_t;

bool aabb_contains(aabb_t aabb, unsigned int x, unsigned int y);
aabb_intersection_t aabb_intersection(aabb_t aabb1, aabb_t aabb2);
bool aabb_is_single(aabb_t aabb);
