#include "color.h"
#include <math.h>

#define BYTE2FLOAT(A) ((float) A) / 256
#define FLOAT2BYTE(A) ((uint8_t)(A * 256))

#define BLEND_G(A) (A <= 0.5) ? ((((16 * A) - 12) * A + 4) * A) : \
                                (sqrt(A)) \

#define BLEND(A, B) (b <= 0.5) ? (A - (1 - (2 * B)) * A * (1 - A)) : \
                                 (A + ((2 * B) - 1) * (BLEND_G(A) - A)) \



bool color_equals(color_t a, color_t b) {
    return (a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a);
}

color_t color_blend(color_t a, color_t b) {
    float alpha = a.a + b.a * (1 - a.a);

    return (color_t) {
        .r = ((a.r * a.a) + (b.r * b.a) * (1 - a.a)) / alpha,
        .g = ((a.g * a.a) + (b.g * b.a) * (1 - a.a)) / alpha,
        .b = ((a.b * a.a) + (b.b * b.a) * (1 - a.a)) / alpha,
        .a = alpha
    };
}
