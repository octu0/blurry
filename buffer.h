#ifdef __APPLE__
#include "libruntime_osx.h"
#elif __linux__
#include "libruntime_linux.h"
#endif

extern halide_buffer_t *create_rgba_buffer(unsigned char *data, int32_t width, int32_t height);
extern halide_buffer_t *create_uint16_array_buffer(unsigned char *data, int32_t width, int32_t height);
extern halide_buffer_t *create_int32_array_buffer(unsigned char *data, int32_t width, int32_t height);
extern halide_buffer_t *create_float_array_buffer(unsigned char *data, int32_t width, int32_t height);
extern halide_buffer_t *create_double_array_buffer(unsigned char *data, int32_t width, int32_t height);
extern void init_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions);
extern void free_buf(halide_buffer_t *buf);

void init_rgba_dim(halide_dimension_t *dim, int32_t width, int32_t height);
void init_array_dim(halide_dimension_t *dim, int32_t width, int32_t height);

void init_rgba_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions);
void init_uint16_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions);
void init_int32_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions);
void init_float_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions);
void init_double_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions);
