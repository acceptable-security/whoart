#include "image.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

image_t* image_init(const char* name, unsigned int width, unsigned int height) {
    image_t* image = (image_t*) malloc(sizeof(image_t));

    if ( image == NULL ) {
        return NULL;
    }

    image->name = name;
    image->width = width;
    image->height = height;
    image->layer_cnt = 0;
    image->layer_alloc = 8;
    image->layers = (layer_t**) malloc(image->layer_alloc * sizeof(layer_t*));

    if ( image->layers == NULL ) {
        free(image);
        return NULL;
    }

    memset(image->layers, 0, image->layer_alloc * sizeof(layer_t*));

    image->composite_layer = layer_init(LAYER_IMAGE, image->width, image->height);

    return image;
}

bool image_new_layer(image_t** _image, layer_type_t type) {
    image_t* image = *_image;

    if ( _image == NULL ) {
        return false;
    }

    layer_t* layer = layer_init(type, image->width, image->height);

    if ( layer == NULL ) {
        return false;
    }

    if ( image->layer_cnt >= image->layer_alloc ) {
        image->layer_alloc += 8;
        layer_t** tmp = realloc(image->layers, image->layer_alloc * sizeof(layer_t*));

        if ( tmp == NULL ) {
            free(tmp);
            free(image);
            *_image = NULL;
            return false;
        }

        image->layers = tmp;
    }

    image->layers[image->layer_cnt++] = layer;
    return true;
}

bool image_composite(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return false;
    }

    if ( image->composite_layer == NULL ) {
        image->composite_layer = layer_init(LAYER_IMAGE, image->width, image->height);
    }

    if ( image->layer_cnt > 0 ) {
        for ( int x = 0; x < image->width; x++ ) {
            for ( int y = 0; y < image->height; y++ ) {
                color_t color = (color_t) {
                    .r = 1.0,
                    .g = 1.0,
                    .b = 1.0,
                    .a = 1.0
                };

                for ( int i = 0; i < image->layer_cnt; i++ ) {
                    color = color_blend(color, layer_get_pixel(image->layers[i], x, y));
                }

                layer_put_pixel(image->composite_layer, x, y, color);
            }
        }

        for ( int i = 0; i < image->layer_cnt; i++ ) {
            image->layers[i]->dirty = false;
        }
    }

    return true;
}

bool image_needs_composite(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return false;
    }

    for ( int i = 0; i < image->layer_cnt; i++ ) {
        if ( image->layers[i]->dirty ) {
            return true;
        }
    }

    return false;
}

unsigned int image_render(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return 0;
    }

    if ( image->composite_layer->textureID == 0 ) {
        printf("Creating a new texture...\n");
        if ( !layer_new_texture(image->composite_layer) ) {
            return 0;
        }
    }

    if ( image_needs_composite(_image) ) {
        printf("Compositing...\n");
        image_composite(_image);

        printf("Transfering texture...\n");
        if ( !layer_transfer_texture(image->composite_layer) ) {
            return 0;
        }
    }

    // printf("Resulting textureID %d\n", image->composite_layer->textureID);
    return image->composite_layer->textureID;
}

void image_clean(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return;
    }

    if ( image->layers != NULL ) {
        for ( int i = 0; i < image->layer_cnt; i++ ) {
            layer_clean(image->layers[i]);
        }

        free(image->layers);
    }

    if ( image->composite_layer != NULL ) {
        layer_clean(image->composite_layer);
    }

    free(image);
}
