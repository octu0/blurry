#ifdef __APPLE__
#include "libruntime_osx.h"

#elif __linux__
#include "libruntime_linux.h"

#endif

#include <stdlib.h>

const struct halide_type_t halide_int16_t = { halide_type_int, 16, 1 };
const struct halide_type_t halide_uint8_t = { halide_type_uint,  8, 1 };

void bind_rgba_buf(halide_buffer_t* buffer, unsigned char *data, int32_t width, int32_t height) {
  buffer->dimensions = 3;
  buffer->dim = (halide_dimension_t *)calloc(buffer->dimensions, sizeof(halide_dimension_t));
  buffer->device = 0;
  buffer->device_interface = NULL;
  buffer->host = data;
  buffer->flags = halide_buffer_flag_host_dirty;

  // width
  buffer->dim[0].min = 0;
  buffer->dim[0].extent = width;
  buffer->dim[0].stride = 4;
  buffer->dim[0].flags = 0;

  // height
  buffer->dim[1].min = 0;
  buffer->dim[1].extent = height;
  buffer->dim[1].stride = width * 4;
  buffer->dim[1].flags = 0;

  // channel
  buffer->dim[2].min = 0;
  buffer->dim[2].extent = 4;
  buffer->dim[2].stride = 1;
  buffer->dim[2].flags = 0;

  buffer->type = halide_uint8_t;
}

void free_buf(halide_buffer_t* buffer) {
  free(buffer->dim);
}
