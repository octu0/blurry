#ifdef __APPLE__
#include "libruntime_osx.h"
#elif __linux__
#include "libruntime_linux.h"
#endif

extern halide_buffer_t *create_rgba_buffer(unsigned char *data, int32_t width, int32_t height);
extern void free_buf(halide_buffer_t *buf);

void init_rgba_dim(halide_dimension_t *dim, int32_t width, int32_t height);
void init_rgba_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions);
