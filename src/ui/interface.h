typedef void (*callback_t)();

int window_init(int width, int height);
void window_set_font(const char* filename, int size);
void window_error(int err, const char* string);
void window_loop(callback_t callback);
void window_close();
