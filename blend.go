package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lblend_normal_osx -lblend_sub_osx -lblend_add_osx -lblend_diff_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lblend_normal_linux -lblend_sub_linux -lblend_add_linux -lblend_diff_linux -ldl -lm
#include <stdlib.h>

#ifdef __APPLE__
#include "libblend_normal_osx.h"
#include "libblend_add_osx.h"
#include "libblend_sub_osx.h"
#include "libblend_diff_osx.h"
#elif __linux__
#include "libblend_normal_linux.h"
#include "libblend_add_linux.h"
#include "libblend_sub_linux.h"
#include "libblend_diff_linux.h"
#endif

#include "buffer.h"

int call_blend(
  uint8_t mode,
  halide_buffer_t *src0,
  int32_t width0, int32_t height0,
  halide_buffer_t *src1,
  int32_t width1, int32_t height1,
  int32_t px, int32_t py,
  halide_buffer_t *out
) {
  if(mode == 0) {
    return blend_normal(src0, width0, height0, src1, width1, height1, px, py, out);
  }
  if(mode == 1) {
    return blend_sub(src0, width0, height0, src1, width1, height1, px, py, out);
  }
  if(mode == 2) {
    return blend_add(src0, width0, height0, src1, width1, height1, px, py, out);
  }
  if(mode == 3) {
    return blend_diff(src0, width0, height0, src1, width1, height1, px, py, out);
  }
  return 1; // not found
}

int libblend(
  unsigned char *src0, int32_t width0, int32_t height0,
  unsigned char *src1, int32_t width1, int32_t height1,
  int32_t px, int32_t py, uint8_t mode,
  unsigned char *out
) {
  halide_buffer_t *in0_rgba_buf = create_rgba_buffer(src0, width0, height0);
  if(in0_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *in1_rgba_buf = create_rgba_buffer(src1, width1, height1);
  if(in1_rgba_buf == NULL){
    free_buf(in0_rgba_buf);
    return 1;
  }

  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width0, height0);
  if(out_rgba_buf == NULL){
    free_buf(in0_rgba_buf);
    free_buf(in1_rgba_buf);
    return 1;
  }

  int ret = call_blend(
    mode,
    in0_rgba_buf, width0, height0,
    in1_rgba_buf, width1, height1,
    px, py,
    out_rgba_buf
  );
  free_buf(in0_rgba_buf);
  free_buf(in1_rgba_buf);
  free_buf(out_rgba_buf);
  return ret;
}
*/
import "C"
import (
	"errors"
	"image"
)

type BlendMode uint8

const (
	BlendNormal BlendMode = iota
	BlendSub
	BlendAdd
	BlendDiff
)

var (
	ErrBlend = errors.New("blend cgo call error")
)

func Blend(src0 *image.RGBA, src1 *image.RGBA, pt image.Point, mode BlendMode) (*image.RGBA, error) {
	width0, height0 := wh(src0)
	width1, height1 := wh(src1)
	out := GetRGBA(width0, height0)

	ret := C.libblend(
		(*C.uchar)(&src0.Pix[0]),
		C.int(width0),
		C.int(height0),
		(*C.uchar)(&src1.Pix[0]),
		C.int(width1),
		C.int(height1),
		C.int(pt.X),
		C.int(pt.Y),
		C.uchar(mode),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrBlend
	}
	return out, nil
}
