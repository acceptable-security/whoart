#include <stdlib.h>
#include <stdio.h>
#include "quadtree/quadtree.h"
#include "images/image.h"
#include "ui/interface.h"

int quadtree_test() {
    quadtree_t* qt = quadtree_init((aabb_t) {
        .minX = 0,
        .minY = 0,
        .maxX = 200,
        .maxY = 200
    }, 4);

    if ( qt == NULL ) {
        printf("Failed to setup quadtree\n.");
        return -1;
    }

    if (!quadtree_insert(qt, 5, 5, "Test1")) { printf("Failed to insert into tree1\n"); return -1; }
    if (!quadtree_insert(qt, 4, 6, "Test2")) { printf("Failed to insert into tree2\n"); return -1; }
    if (!quadtree_insert(qt, 6, 7, "Test3")) { printf("Failed to insert into tree3\n"); return -1; }
    if (!quadtree_insert(qt, 8, 8, "Test4")) { printf("Failed to insert into tree4\n"); return -1; }
    if (!quadtree_insert(qt, 20, 9, "Test5")) { printf("Failed to insert into tree5\n"); return -1; }
    if (!quadtree_insert(qt, 150, 9, "Test6")) { printf("Failed to insert into tree6\n"); return -1; }

    // quadtree_debug(qt, 0);

    // printf("%s\n", (const char*) quadtree_search(qt, 20, 9));

    quadtree_clean(qt);

    return 0;
}

int interface_test() {
    window_init(800, 600);
    window_set_font("res/fonts/DroidSans.ttf", 14);
    window_loop(NULL);
    window_close();
    return 0;
}

int main(int argc, char* argv[]) {
    image_t* image = image_init("Image 1", 64, 64);
    image_new_layer(&image, LAYER_DRAWING);
    image_new_layer(&image, LAYER_IMAGE);

    for ( int i = 0; i < 40; i++ ) {
        layer_put_pixel(image->layers[0], i, i, (color_t){
            .r = 0.5,
            .g = 0.5,
            .b = 0.5,
            .a = 1.0
        });
    }

    for ( int i = 0; i < 40; i++ ) {
        layer_put_pixel(image->layers[0], i, 64 - i, (color_t){
            .r = 1.0,
            .g = 0,
            .b = 0,
            .a = 1.0
        });
    }

    window_init(800, 600);
    window_set_font("res/fonts/DroidSans.ttf", 14);
    window_add_image(image);
    window_set_image_scale(0, 5.0);
    window_loop(NULL);
    window_close();

    return 0;
}
