#include "layer.h"
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../../../lib/stb_image.h"


layer_t* layer_init(layer_type_t type, unsigned int width, unsigned int height, const char* name) {
    layer_t* layer = (layer_t*) malloc(sizeof(layer_t));

    if ( layer == NULL ) {
        return NULL;
    }

    layer->name = name;

    layer->visible = true;
    layer->type = type;
    layer->width = width;
    layer->height = height;

    switch ( type ) {
        case LAYER_IMAGE:
            layer->image_data = (layer_image_data_t*) malloc(sizeof(layer_image_data_t));

            if ( layer->image_data == NULL ) {
                free(layer);
                return NULL;
            }

            layer->image_data->color_data = (color_t*) malloc((width * height) * sizeof(color_t));

            return layer;

        case LAYER_DRAWING:
            layer->drawing_data = (layer_drawing_data_t*) malloc(sizeof(layer_drawing_data_t));

            if ( layer->drawing_data == NULL ) {
                free(layer);
                return NULL;
            }

            layer->drawing_data->qt = quadimage_init((aabb_t){ .minX = 0, .minY = 0, .maxX = width, .maxY = height});

            return layer;

        case LAYER_COLOR:
            layer->color_data = (layer_color_data_t*) malloc(sizeof(layer_color_data_t));

            if ( layer->color_data == NULL ) {
                free(layer);
                return NULL;
            }

            return layer;
    }
}

void layer_put_pixel(layer_t* layer, unsigned int x, unsigned int y, color_t color) {
    layer->dirty = true;

    if ( layer->type == LAYER_IMAGE ) {
        layer->image_data->color_data[(y * layer->width) + x] = color;
    }
    else if ( layer->type == LAYER_DRAWING ) {
        quadimage_insert_point(layer->drawing_data->qt, x, y, color);
    }
    else if ( layer->type == LAYER_COLOR ) {
        layer->color_data->color = color;
    }
}

color_t layer_get_pixel(layer_t* layer, unsigned int x, unsigned int y) {
    if ( layer->type == LAYER_COLOR ) {
        return layer->color_data->color;
    }
    else if ( layer->type == LAYER_IMAGE ) {
        return layer->image_data->color_data[(y * layer->width) + x];
    }
    else if ( layer->type == LAYER_DRAWING ) {
        return quadimage_search(layer->drawing_data->qt, x, y);
    }
    else {
        return (color_t) {};
    }
}

bool layer_load_image(layer_t* layer, const char* filename) {
    if ( layer->type == LAYER_COLOR ) {
        return false;
    }

    int width;
    int height;
    int components;

    unsigned char* data = stbi_load(filename, &width, &height, &components, 4);

    if ( data == NULL ) {
        return false;
    }

    if ( layer->type == LAYER_IMAGE ) {
        if ( layer->image_data->color_data == NULL ) {
            layer->image_data->color_data = (color_t*) malloc((width * height) * sizeof(color_t));
        }
    }
    else if ( layer->type == LAYER_DRAWING ) {
        layer->drawing_data->qt = quadimage_init((aabb_t){ .minX = 0, .minY = 0, .maxX = width, .maxY = height});
    }

    for ( int i = 0; i < (width * height); i+=4 ) {
        layer_put_pixel(layer, i / width, i % width, (color_t) {
            .r = ((float) data[i + 0]) / 256,
            .g = ((float) data[i + 1]) / 256,
            .b = ((float) data[i + 2]) / 256,
            .a = ((float) data[i + 3]) / 256
        });
    }

    stbi_image_free(data);

    return true;
}

bool layer_new_texture(layer_t* layer) {
    if ( layer->textureID == 0 ) {
        glGenTextures(1, &layer->textureID);
    }

    glBindTexture(GL_TEXTURE_2D, layer->textureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    return layer_transfer_texture(layer);
}

bool layer_transfer_texture(layer_t* layer) {
    if ( layer->textureID == 0 ) {
        return false;
    }

    switch ( layer->type ) {
        case LAYER_IMAGE:
            glBindTexture(GL_TEXTURE_2D, layer->textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layer->width, layer->height, 0, GL_RGBA, GL_FLOAT, layer->image_data->color_data);
            break;

        default:
            break;
    }

    return true;
}

void layer_clean(layer_t* layer) {
    switch ( layer->type ) {
        case LAYER_COLOR:
            free(layer->color_data);
            break;

        case LAYER_IMAGE:
            free(layer->image_data->color_data);
            free(layer->image_data);
            break;

        case LAYER_DRAWING:
            quadimage_clean(layer->drawing_data->qt);
            free(layer->drawing_data);
            break;
    }

    free(layer);
}
