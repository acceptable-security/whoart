#include <stdlib.h>
#include <stdio.h>
#include "quadtree/quadtree.h"

int main(int argc, char* argv[]) {
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
}
