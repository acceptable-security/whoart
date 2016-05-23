#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "interface.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "../../lib/nuklear.h"
#include "../../lib/nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

typedef struct nk_image nk_image_t;

#define GET_IMG(I) ((image_data_t*) global_context.images->items[I])

typedef struct {
    image_t* image;
    nk_image_t nk_image;
    list_t* layers;
} image_data_t;

void image_data_clean(image_data_t* data);

typedef struct {
    int width;
    int height;
    GLFWwindow* win;
    struct nk_context* ctx;

    list_t* images;
} window_context_t;

window_context_t global_context; // THIS IS BAD

int window_init(int width, int height) {
    global_context.width = width;
    global_context.height = height;

    glfwSetErrorCallback(window_error);

    if ( !glfwInit() ) {
        fprintf(stderr, "[GLFW] Failed to initiaize.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    global_context.win = glfwCreateWindow(width, height, "whoart", NULL, NULL);
    glfwMakeContextCurrent(global_context.win);
    glfwGetWindowSize(global_context.win, &global_context.width, &global_context.height);

    glViewport(0, 0, global_context.width, global_context.height);

    glewExperimental = 1;

    if ( glewInit() != GLEW_OK ) {
        fprintf(stderr, "[GLEW] Failed to initialize.\n");
        return 1;
    }

    global_context.ctx = nk_glfw3_init(global_context.win, NK_GLFW3_INSTALL_CALLBACKS);
    global_context.images = list_init(&image_data_clean);

    return 0;
}

void window_add_image(image_t* image) {
    image_data_t* data = (image_data_t*) malloc(sizeof(image_data_t));

    if ( data == NULL ) {
        return;
    }

    data->image = image;
    data->nk_image = nk_image_id((int) image_render(&image));
    data->layers = list_init(free);

    for ( int i = 0; i < image->layers->length; i++ ) {
        layer_t* layer = (layer_t*) image->layers->items[i];
        layer_new_texture(layer);
        nk_image_t _nk_img = nk_image_id(layer->textureID);
        nk_image_t* nk_img = (nk_image_t*) malloc(sizeof(nk_image_t));
        memcpy(nk_img, &_nk_img, sizeof(nk_image_t));
        list_add(&data->layers, nk_img);
    }

    list_add(&global_context.images, data);
}

void window_set_image_scale(int indx, float scale) {
    if ( indx < 0 || indx >= global_context.images->length ) {
        return;
    }

    image_t* image = GET_IMG(indx)->image;
    int width = image->width * scale;
    int height = image->height * scale;
    int tex = image_render(&image);

    nk_image_t nk_img = GET_IMG(indx)->nk_image;

    if ( nk_img.region[2] != width || nk_img.region[3] != height ) {
        GET_IMG(indx)->nk_image = nk_subimage_id(tex, width, height, (struct nk_rect) {
            .x = 0,
            .y = 0,
            .w = width,
            .h = height
        });
    }
}

void window_set_font(const char* filename, int size) {
    struct nk_font_atlas* atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font* font = nk_font_atlas_add_from_file(atlas, filename, size, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(global_context.ctx, &font->handle);
}

void window_render() {
    struct nk_panel layout;

    for ( int i = 0; i < global_context.images->length; i++ ) {
        image_t* image = GET_IMG(i)->image;
        image_render(&image);

        if ( nk_begin(global_context.ctx, &layout, image->name, nk_rect(0, 0, image->width * 2, image->height * 2), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE) ) {
            float size = ((float) GET_IMG(i)->nk_image.region[3]) / ((float) image->height);
            nk_layout_row_dynamic(global_context.ctx, 20, 1);
            nk_labelf(global_context.ctx, NK_TEXT_LEFT, "Zoom: %f", size);
            nk_slider_float(global_context.ctx, 0.5f, &size, 10.0, 0.1f);

            if ( GET_IMG(i)->nk_image.region[2] == 0 ) {
                nk_layout_row_static(global_context.ctx, image->height, image->width, 1);
            }
            else {
                nk_layout_row_static(global_context.ctx, GET_IMG(i)->nk_image.region[3], GET_IMG(i)->nk_image.region[2], 1);
            }


            window_set_image_scale(i, size);
            nk_image(global_context.ctx, GET_IMG(i)->nk_image);

        }
        nk_end(global_context.ctx);

        if ( nk_begin(global_context.ctx, &layout, "Layers", nk_rect(image->width * 2 + 10, 0, 150, 300), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE) ) {
            nk_layout_row_dynamic(global_context.ctx, 30, 1);

            for ( int j = 0; j < image->layers->length; j++ ) {
                nk_button_image_label(global_context.ctx, *(nk_image_t*)(GET_IMG(i)->layers->items[i]), ((layer_t*) image->layers->items[j])->name, NK_TEXT_CENTERED, NK_BUTTON_DEFAULT);
            }

            nk_layout_row_static(global_context.ctx, 30, 80, 1);
        }

        nk_end(global_context.ctx);
    }
}

void window_loop(callback_t callback) {
    while ( !glfwWindowShouldClose(global_context.win) ) {
        glfwPollEvents();
        nk_glfw3_new_frame();

        window_render();

        if ( callback != NULL ) {
            callback();
        }

        glfwGetWindowSize(global_context.win, &global_context.width, &global_context.height);
        glViewport(0, 0, global_context.width, global_context.height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(global_context.win);
    }
}

void image_data_clean(image_data_t* data) {
    if ( data == NULL ) {
        return;
    }

    if ( data->image != NULL ) {
        image_clean(&data->image);
    }

    if ( data->layers != NULL ) {
        list_clean(&data->layers);
    }

    free(data);
}

void window_close() {
    if ( global_context.images != NULL ) {
        list_clean(&global_context.images);
    }

    nk_glfw3_shutdown();
    glfwTerminate();
}

void window_error(int err, const char* string) {
    fprintf(stderr, "Error %d: %s\n", err, string);
}
