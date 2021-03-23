#ifdef __APPLE__
#include "libruntime_osx.h"

#elif __linux__
#include "libruntime_linux.h"

#endif

extern void bind_rgba_buf(halide_buffer_t* buf, unsigned char *data, int32_t width, int32_t height);
extern void free_buf(halide_buffer_t* buf);
