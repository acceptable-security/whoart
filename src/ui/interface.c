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

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "../../lib/nuklear_glfw_gl3.h"

#include "interface.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

typedef struct {
    int width;
    int height;
    GLFWwindow* win;
    struct nk_context* ctx;
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

    return 0;
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

    if ( nk_begin(global_context.ctx, &layout, "whoart", nk_rect(50, 50, 230, 250), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE) ) {
        nk_layout_row_static(global_context.ctx, 30, 80, 1);

        if ( nk_button_label(global_context.ctx, "button", NK_BUTTON_DEFAULT) ) {
            fprintf(stdout, "button pressed\n");
        }
    }
    nk_end(global_context.ctx);
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

void window_close() {
    nk_glfw3_shutdown();
    glfwTerminate();
}

void window_error(int err, const char* string) {
    fprintf(stderr, "Error %d: %s\n", err, string);
}
