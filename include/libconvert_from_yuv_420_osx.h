#ifndef HALIDE__libconvert_from_yuv_420_osx_h
#define HALIDE__libconvert_from_yuv_420_osx_h
#include <stdint.h>

// Forward declarations of the types used in the interface
// to the Halide pipeline.
//
// For the definitions of these structs, include HalideRuntime.h

// Halide's representation of a multi-dimensional array.
// Halide::Runtime::Buffer is a more user-friendly wrapper
// around this. Its declaration is in HalideBuffer.h
struct halide_buffer_t;

// Metadata describing the arguments to the generated function.
// Used to construct calls to the _argv version of the function.
struct halide_filter_metadata_t;

#ifndef HALIDE_MUST_USE_RESULT
#ifdef __has_attribute
#if __has_attribute(nodiscard)
#define HALIDE_MUST_USE_RESULT [[nodiscard]]
#elif __has_attribute(warn_unused_result)
#define HALIDE_MUST_USE_RESULT __attribute__((warn_unused_result))
#else
#define HALIDE_MUST_USE_RESULT
#endif
#else
#define HALIDE_MUST_USE_RESULT
#endif
#endif

#ifndef HALIDE_FUNCTION_ATTRS
#define HALIDE_FUNCTION_ATTRS
#endif



#ifdef __cplusplus
extern "C" {
#endif

HALIDE_FUNCTION_ATTRS
int convert_from_yuv_420_par_for_convert_from_yuv_420_s0_ch_par_for_convert_from_yuv_420_s0_x_xo_ti_ti(void *__user_context, int32_t _convert_from_yuv_420_s0_x_xo_ti_ti, uint8_t *_closure_arg__27);
HALIDE_FUNCTION_ATTRS
int convert_from_yuv_420_par_for_convert_from_yuv_420_s0_ch_par_for_convert_from_yuv_420_s0_x_xo_ti_ti__1(void *__user_context, int32_t _convert_from_yuv_420_s0_x_xo_ti_ti, uint8_t *_closure_arg__28);
HALIDE_FUNCTION_ATTRS
int convert_from_yuv_420_par_for_convert_from_yuv_420_s0_ch(void *__user_context, int32_t _convert_from_yuv_420_s0_ch, uint8_t *_closure_arg__26);
HALIDE_FUNCTION_ATTRS
int convert_from_yuv_420(struct halide_buffer_t *_yuv_y_buffer, struct halide_buffer_t *_yuv_u_buffer, struct halide_buffer_t *_yuv_v_buffer, int32_t _width, int32_t _height, struct halide_buffer_t *_convert_from_yuv_420_buffer);

HALIDE_FUNCTION_ATTRS
int convert_from_yuv_420_argv(void **args);

HALIDE_FUNCTION_ATTRS
const struct halide_filter_metadata_t *convert_from_yuv_420_metadata();

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
