#ifndef HALIDE__libmorphology_open_linux_h
#define HALIDE__libmorphology_open_linux_h
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
int morphology_open_par_for_in_30_s0_y_rebased__1(void *__user_context, int32_t _in__30_s0_y_rebased, uint8_t *_closure_arg__192);
HALIDE_FUNCTION_ATTRS
int morphology_open_par_for_erode_tmp_s0_y_y__1(void *__user_context, int32_t _erode_tmp_s0_y_y, uint8_t *_closure_arg__193);
HALIDE_FUNCTION_ATTRS
int morphology_open_par_for_dilate_tmp_s0_y_y__1(void *__user_context, int32_t _dilate_tmp_s0_y_y, uint8_t *_closure_arg__194);
HALIDE_FUNCTION_ATTRS
int morphology_open_par_for_morphology_open_s0_ch_par_for_morphology_open_s0_x_xo_ti_ti__2(void *__user_context, int32_t _morphology_open_s0_x_xo_ti_ti, uint8_t *_closure_arg__196);
HALIDE_FUNCTION_ATTRS
int morphology_open_par_for_morphology_open_s0_ch_par_for_morphology_open_s0_x_xo_ti_ti__3(void *__user_context, int32_t _morphology_open_s0_x_xo_ti_ti, uint8_t *_closure_arg__197);
HALIDE_FUNCTION_ATTRS
int morphology_open_par_for_morphology_open_s0_ch__1(void *__user_context, int32_t _morphology_open_s0_ch, uint8_t *_closure_arg__195);
HALIDE_FUNCTION_ATTRS
int morphology_open(struct halide_buffer_t *_src_buffer, int32_t _width, int32_t _height, int32_t _size, struct halide_buffer_t *_morphology_open_buffer);

HALIDE_FUNCTION_ATTRS
int morphology_open_argv(void **args);

HALIDE_FUNCTION_ATTRS
const struct halide_filter_metadata_t *morphology_open_metadata();

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
