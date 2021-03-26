#ifdef __APPLE__
#include "libruntime_osx.h"
#elif __linux__
#include "libruntime_linux.h"
#endif

#include <stdlib.h>
#include "bridge.h"

const struct halide_type_t halide_int16_t = { halide_type_int, 16, 1 };
const struct halide_type_t halide_uint8_t = { halide_type_uint,  8, 1 };

halide_buffer_t *create_rgba_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 3;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free(buffer);
    return NULL;
  }

  init_rgba_dim(dim, width, height);
  init_rgba_buf(buffer, dim, data, dimensions);

  return buffer;
}

void init_rgba_dim(halide_dimension_t *dim, int32_t width, int32_t height) {
  // width
  dim[0].min = 0;
  dim[0].extent = width;
  dim[0].stride = 4;
  dim[0].flags = 0;

  // height
  dim[1].min = 0;
  dim[1].extent = height;
  dim[1].stride = width * 4;
  dim[1].flags = 0;

  // channel
  dim[2].min = 0;
  dim[2].extent = 4;
  dim[2].stride = 1;
  dim[2].flags = 0;
}

void init_rgba_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  buffer->dimensions = dimensions;
  buffer->dim = dim;
  buffer->device = 0;
  buffer->device_interface = NULL;
  buffer->host = data;
  buffer->flags = halide_buffer_flag_host_dirty;
  buffer->type = halide_uint8_t;
}

void free_buf(halide_buffer_t *buffer) {
  if(buffer != NULL) {
    free(buffer->dim);
  }
  free(buffer);
}
