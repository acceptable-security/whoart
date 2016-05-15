#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float r;
    float g;
    float b;
    float a;
}__attribute__((__packed__)) color_t;

color_t color_blend(color_t a, color_t b);
bool color_equals(color_t a, color_t b);
