#include "../images/image.h"
typedef void (*callback_t)();

int window_init(int width, int height);
void window_add_image(image_t* image);
void window_set_font(const char* filename, int size);
void window_error(int err, const char* string);
void window_set_image_scale(int indx, float scale);
void window_loop(callback_t callback);
void window_close();
