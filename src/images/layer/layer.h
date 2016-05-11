#include "../color/color.h"
#include "../quadimage/quadimage.h"

typedef struct {
    union {
        color64_t** c64_data;
        color32_t** c32_data;
    };
    uint8_t size : 1;

    unsigned int width;
    unsigned int height;
} layer_image_data_t;

typedef struct {
    quadimage_t* qt;

    unsigned int width;
    unsigned int height;
} layer_drawing_data_t;

typedef struct {
    union {
        color64_t c64;
        color32_t c32;
    };

    uint8_t size : 1;
} layer_color_data_t;

typedef enum {
    LAYER_IMAGE,
    LAYER_DRAWING,
    LAYER_COLOR
} layer_type_t;

typedef struct {
    layer_type_t type;

    union {
        layer_image_data_t* image_data;
        layer_drawing_data_t* drawing_data;
        layer_color_data_t* color_data;
    };
} layer_t;
