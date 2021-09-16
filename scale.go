package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lscale_osx -lscale_box_osx -lscale_linear_osx -lscale_gaussian_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lscale_linux -lscale_box_linux -lscale_linear_linux -lscale_gaussian_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libscale_osx.h"
#include "libscale_box_osx.h"
#include "libscale_linear_osx.h"
#include "libscale_gaussian_osx.h"
#elif __linux__
#include "libscale_linux.h"
#include "libscale_box_linux.h"
#include "libscale_linear_linux.h"
#include "libscale_gaussian_linux.h"
#endif

#include "buffer.h"

int call_scale(
  uint8_t mode,
  halide_buffer_t *src,
  int32_t width, int32_t height,
  int32_t scale_width, int32_t scale_height,
  halide_buffer_t *out
) {
  if(mode == 0) {
    return scale(src, width, height, scale_width, scale_height, out);
  }
  if(mode == 1) {
    return scale_box(src, width, height, scale_width, scale_height, out);
  }
  if(mode == 2) {
    return scale_linear(src, width, height, scale_width, scale_height, out);
  }
  if(mode == 3) {
    return scale_gaussian(src, width, height, scale_width, scale_height, out);
  }
  return 1; // not found
}

int libscale(
  unsigned char *src, int32_t width, int32_t height,
  int32_t scale_width, int32_t scale_height,
  uint8_t mode,
  unsigned char *out
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, scale_width, scale_height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = call_scale(mode, in_rgba_buf, width, height, scale_width, scale_height, out_rgba_buf);
  free_buf(in_rgba_buf);
  free_buf(out_rgba_buf);
  return ret;
}
*/
import "C"
import (
	"errors"
	"image"
)

type ScaleFilter uint8

const (
	ScaleFilterNone ScaleFilter = iota
	ScaleFilterBox
	ScaleFilterLinear
	ScaleFilterGaussian
)

var (
	ErrScale = errors.New("scale cgo call error")
)

func Scale(img *image.RGBA, scaleWidth, scaleHeight int, mode ScaleFilter) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(scaleWidth, scaleHeight)

	ret := C.libscale(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.int(scaleWidth),
		C.int(scaleHeight),
		C.uchar(mode),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrCrop
	}
	return out, nil
}
