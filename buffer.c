#ifdef __APPLE__
#include "libruntime_osx.h"
#elif __linux__
#include "libruntime_linux.h"
#endif

#include <stdlib.h>
#include "buffer.h"

const struct halide_type_t halide_int16_t = { halide_type_int,  16, 1 };
const struct halide_type_t halide_int32_t = { halide_type_int, 32, 1 };
const struct halide_type_t halide_uint8_t = { halide_type_uint,  8, 1 };
const struct halide_type_t halide_uint16_t = { halide_type_uint, 16, 1 };
const struct halide_type_t halide_float_t = { halide_type_float, 32, 1 };
const struct halide_type_t halide_double_t = { halide_type_float, 64, 1 };

halide_buffer_t *create_rgba_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 3;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_rgba_dim(dim, width, height);
  init_uint8_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_yuv_plane_buffer(unsigned char *data, int32_t stride, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_yuv_dim(dim, stride, width, height);
  init_uint8_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_yuv420_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  int32_t y_width = width;
  int32_t y_height = height;
  int32_t u_height = y_height / 2;
  int32_t v_height = y_height / 2;
  int32_t stride = y_width; // square output(uv ends in zero)

  init_yuv_dim(dim, stride, y_width, y_height + u_height + v_height);
  init_uint8_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_yuv444_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  int32_t y_width = width;
  int32_t y_height = height;
  int32_t u_height = y_height;
  int32_t v_height = y_height;
  int32_t stride = y_width;

  init_yuv_dim(dim, stride, y_width, y_height + u_height + v_height);
  init_uint8_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_uint8_array_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_array_dim(dim, width, height);
  init_uint8_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_uint16_array_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_array_dim(dim, width, height);
  init_uint16_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_int32_array_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_array_dim(dim, width, height);
  init_int32_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_float_array_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_array_dim(dim, width, height);
  init_float_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_double_array_buffer(unsigned char *data, int32_t width, int32_t height) {
  int32_t dimensions = 2;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_array_dim(dim, width, height);
  init_double_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_pcm16_buffer(unsigned char *data, int32_t length) {
  int32_t dimensions = 1;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  halide_dimension_t *dim = (halide_dimension_t *) malloc(dimensions * sizeof(halide_dimension_t));
  if(dim == NULL) {
    free_buf(buffer);
    return NULL;
  }
  memset(dim, 0, dimensions * sizeof(halide_dimension_t));

  init_1darray_dim(dim, length);
  init_int16_buf(buffer, dim, data, dimensions);

  return buffer;
}

halide_buffer_t *create_pcm16_decibel_buffer(unsigned char *data, int32_t length) {
  int32_t dimensions = 0;
  halide_buffer_t *buffer = (halide_buffer_t *) malloc(sizeof(halide_buffer_t));
  if(buffer == NULL) {
    return NULL;
  }
  memset(buffer, 0, sizeof(halide_buffer_t));

  init_float_buf(buffer, NULL, data, dimensions);

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

void init_yuv_dim(halide_dimension_t *dim, int32_t stride, int32_t width, int32_t height) {
  // width
  dim[0].min = 0;
  dim[0].extent = width;
  dim[0].stride = 1;
  dim[0].flags = 0;

  // height
  dim[1].min = 0;
  dim[1].extent = height;
  dim[1].stride = stride;
  dim[1].flags = 0;
}

void init_array_dim(halide_dimension_t *dim, int32_t width, int32_t height) {
  // width
  dim[0].min = 0;
  dim[0].extent = width;
  dim[0].stride = 1;
  dim[0].flags = 0;

  // height
  dim[1].min = 0;
  dim[1].extent = height;
  dim[1].stride = width;
  dim[1].flags = 0;
}

void init_1darray_dim(halide_dimension_t *dim, int32_t length) {
  dim[0].min = 0;
  dim[0].extent = length;
  dim[0].stride = 1;
  dim[0].flags = 0;
}

void init_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  buffer->dimensions = dimensions;
  buffer->dim = dim;
  buffer->device = 0;
  buffer->device_interface = NULL;
  buffer->host = data;
  buffer->flags = halide_buffer_flag_host_dirty;
}

void init_uint8_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  init_buf(buffer, dim, data, dimensions);
  buffer->type = halide_uint8_t;
}

void init_uint16_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  init_buf(buffer, dim, data, dimensions);
  buffer->type = halide_uint16_t;
}

void init_int16_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  init_buf(buffer, dim, data, dimensions);
  buffer->type = halide_int16_t;
}

void init_int32_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  init_buf(buffer, dim, data, dimensions);
  buffer->type = halide_int32_t;
}

void init_float_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  init_buf(buffer, dim, data, dimensions);
  buffer->type = halide_float_t;
}

void init_double_buf(halide_buffer_t *buffer, halide_dimension_t *dim, unsigned char *data, int32_t dimensions) {
  init_buf(buffer, dim, data, dimensions);
  buffer->type = halide_double_t;
}

void free_buf(halide_buffer_t *buffer) {
  if(buffer != NULL) {
    free(buffer->dim);
  }
  free(buffer);
}
