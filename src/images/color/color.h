#include <stdint.h>
#include <stdbool.h>

typedef union {
    float r;
    float g;
    float b;
    float a;
} color_t;

color_t color_blend(color_t a, color_t b);
bool color_equals(color_t a, color_t b);
