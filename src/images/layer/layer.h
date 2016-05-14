#include "../quadimage/quadimage.h"

typedef struct {
    color_t* color_data;
} layer_image_data_t;

typedef struct {
    quadimage_t* qt;
} layer_drawing_data_t;

typedef struct {
    color_t color;
} layer_color_data_t;

typedef enum {
    LAYER_IMAGE,
    LAYER_DRAWING,
    LAYER_COLOR
} layer_type_t;

typedef struct {
    layer_type_t type;
    unsigned int width;
    unsigned int height;

    union {
        layer_image_data_t* image_data;
        layer_drawing_data_t* drawing_data;
        layer_color_data_t* color_data;
    };
} layer_t;
