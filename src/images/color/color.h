#include <stdint.h>

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } components;

    uint32_t bits;
} color32_t;

typedef union {
    struct {
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t a;
    } components;

    uint64_t bits;
} color64_t;

color64_t color32_to_color64(color32_t color);
color32_t color64_to_color32(color64_t color);
