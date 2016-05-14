#include "layer/layer.h"

typedef struct {
    unsigned int textureID;

    unsigned int width;
    unsigned int height;

    layer_t* composite_layer;

    unsigned int layer_cnt;
    unsigned int layer_alloc;
    layer_t** layers;
} image_t;

image_t* image_init(unsigned int width, unsigned int height);
bool image_new_layer(image_t** image, layer_type_t type);
bool image_composite(image_t** _image);
bool image_new_texture(image_t** _image);
bool image_transfer_texture(image_t** _image);
bool image_needs_composite(image_t** _image);
unsigned int image_render(image_t** _image);
