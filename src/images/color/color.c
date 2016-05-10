#include "color.h"

color64_t color32_to_color64(color32_t color) {
    return (color64_t) {
        .components = {
            .r = color.components.r << 8,
            .g = color.components.g << 8,
            .b = color.components.b << 8,
            .a = color.components.a << 8
        }
    };
}

color32_t color64_to_color32(color64_t color) {
    return (color32_t) {
        .components = {
            .r = color.components.r >> 8,
            .g = color.components.g >> 8,
            .b = color.components.b >> 8,
            .a = color.components.a >> 8
        }
    };
}
