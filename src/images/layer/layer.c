#include "layer.h"
#include <stdlib.h>

layer_t* layer_init(layer_type_t type) {
    layer_t* layer = (layer_t*) malloc(sizeof(layer_t));

    if ( layer == NULL ) {
        return NULL;
    }

    layer->type = type;

    switch ( type ) {
        case LAYER_IMAGE:
            layer->image_data = (layer_image_data_t*) malloc(sizeof(layer_image_data_t));

            if ( layer->image_data == NULL ) {
                free(layer);
                return NULL;
            }

            return layer;

        case LAYER_DRAWING:
            layer->drawing_data = (layer_drawing_data_t*) malloc(sizeof(layer_drawing_data_t));

            if ( layer->drawing_data == NULL ) {
                free(layer);
                return NULL;
            }

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
