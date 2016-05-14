#include "image.h"
#include <stdlib.h>
#include <string.h>
#include <GLFW/glfw3.h>

image_t* image_init(unsigned int width, unsigned int height) {
    image_t* image = (image_t*) malloc(sizeof(image_t));

    if ( image == NULL ) {
        return NULL;
    }

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
                color_t color = layer_get_pixel(image->layers[0], x, y);

                for ( int i = 1; i < image->layer_cnt; i++ ) {
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

bool image_new_texture(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return false;
    }

    if ( image->textureID == 0 ) {
        glGenTextures(1, &image->textureID);
    }

    glBindTexture(GL_TEXTURE_2D, image->textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return image_transfer_texture(_image);
}

bool image_transfer_texture(image_t** _image) {
    image_t* image = *_image;

    if ( image == NULL ) {
        return false;
    }

    if ( image->textureID == 0 ) {
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, image->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_FLOAT, image->composite_layer->image_data->color_data);

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

    if ( image_needs_composite(_image) ) {
        image_composite(_image);
    }

    if ( image->textureID == 0 ) {
        if ( !image_new_texture(_image) ) {
            return 0;
        }
    }
    else {
        if ( !image_transfer_texture(_image) ) {
            return 0;
        }
    }

    return image->textureID;
}
