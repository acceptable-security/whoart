#include "image.h"
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include <stdio.h>

image_t* image_init(const char* name, unsigned int width, unsigned int height) {
    image_t* image = (image_t*) malloc(sizeof(image_t));

    if ( image == NULL ) {
        return NULL;
    }

    image->name = name;
    image->width = width;
    image->height = height;
    image->layers = list_init(&layer_clean);

    if ( image->layers == NULL ) {
        free(image);
        return NULL;
    }

    image->composite_layer = layer_init(LAYER_IMAGE, image->width, image->height, "");

    return image;
}

bool image_new_layer(image_t** _image, layer_type_t type) {
    image_t* image = *_image;

    if ( _image == NULL ) {
        return false;
    }

    char* name;

    if ( asprintf(&name, "Layer %i", (image->layers->length + 1)) == -1 ) {
        return false;
    }

    layer_t* layer = layer_init(type, image->width, image->height, name);

    if ( layer == NULL ) {
        return false;
    }

    list_add(&image->layers, layer);

    if ( image->layers == NULL ) {
        printf("CLEANING!");
        image_clean(_image);
        return false;
    }

    return true;
}

bool image_composite(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return false;
    }

    if ( image->composite_layer == NULL ) {
        image->composite_layer = layer_init(LAYER_IMAGE, image->width, image->height, "");
    }

    if ( image->layers->length > 0 ) {
        for ( int x = 0; x < image->width; x++ ) {
            for ( int y = 0; y < image->height; y++ ) {
                color_t color = (color_t) {
                    .r = 1.0,
                    .g = 1.0,
                    .b = 1.0,
                    .a = 1.0
                };

                for ( int i = 0; i < image->layers->length; i++ ) {
                    color = color_blend(layer_get_pixel((layer_t*) image->layers->items[i], x, y), color);
                }

                layer_put_pixel(image->composite_layer, x, y, color);
            }
        }

        for ( int i = 0; i < image->layers->length; i++ ) {
            ((layer_t*) image->layers->items[i])->dirty = false;
        }
    }

    return true;
}

bool image_needs_composite(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return false;
    }

    for ( int i = 0; i < image->layers->length; i++ ) {
        if ( ((layer_t*) image->layers->items[i])->dirty ) {
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
        list_clean(&image->layers);
    }

    if ( image->name != NULL && strcmp(image->name, "") != 0 ) {
        free((char*) image->name);
    }

    if ( image->composite_layer != NULL ) {
        layer_clean(image->composite_layer);
    }

    free(image);
    *_image = NULL;
}
