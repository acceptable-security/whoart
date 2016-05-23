#pragma once

#include "../quadimage/quadimage.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_GLFW_GL3_IMPLEMENTATION
#include "../../../lib/nuklear.h"

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
    bool hasTexture;
    unsigned int textureID;

    bool visible;
    bool dirty;

    unsigned int width;
    unsigned int height;

    const char* name;

    union {
        layer_image_data_t* image_data;
        layer_drawing_data_t* drawing_data;
        layer_color_data_t* color_data;
    };
} layer_t;

layer_t* layer_init(layer_type_t type, unsigned int width, unsigned int height, const char* name);
void layer_put_pixel(layer_t* layer, unsigned int x, unsigned int y, color_t color);
color_t layer_get_pixel(layer_t* layer, unsigned int x, unsigned int y);
bool layer_load_image(layer_t* layer, const char* filename);
bool layer_new_texture(layer_t* layer);
bool layer_transfer_texture(layer_t* layer);
void layer_clean(layer_t* layer);
