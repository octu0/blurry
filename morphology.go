package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lmorphology_open_osx -lmorphology_close_osx -lmorphology_gradient_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lmorphology_open_linux -lmorphology_close_linux -lmorphology_gradient_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libmorphology_open_osx.h"
#include "libmorphology_close_osx.h"
#include "libmorphology_gradient_osx.h"
#elif __linux__
#include "libmorphology_open_linux.h"
#include "libmorphology_close_linux.h"
#include "libmorphology_gradient_linux.h"
#endif

int call_morphology(
  unsigned char mode,
  halide_buffer_t *in,
  int32_t width, int32_t height,
  int32_t size,
  halide_buffer_t *out
) {
  if(1 == mode) {
    return morphology_open(in, width, height, size, out);
  }
  if(2 == mode) {
    return morphology_close(in, width, height, size, out);
  }
  if(3 == mode) {
    return morphology_gradient(in, width, height, size, out);
  }
  return 1;
}

int libmorphology(
  unsigned char *src, int32_t width, int32_t height,
  unsigned char mode, int32_t size, int32_t count,
  unsigned char *out
) {
  // 1
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }
  if(call_morphology(mode, in_rgba_buf, width, height, size, out_rgba_buf) != 0) {
    free_buf(in_rgba_buf);
    free_buf(out_rgba_buf);
    return 1;
  }
  free_buf(in_rgba_buf);
  free_buf(out_rgba_buf);

  // 2 .. count
  halide_buffer_t *in_tmp = NULL;
  halide_buffer_t *out_tmp = NULL;
  for(int i = 1; i < count; i+= 1) {
    in_tmp = create_rgba_buffer(out, width, height);
    if(in_tmp == NULL) {
      return 1;
    }
    out_tmp = create_rgba_buffer(out, width, height);
    if(out_tmp == NULL) {
      free_buf(in_tmp);
      return 1;
    }
    if(call_morphology(mode, in_tmp, width, height, size, out_tmp) != 0){
      free_buf(in_tmp);
      free_buf(out_tmp);
      return 1;
    }
    free_buf(in_tmp);
    free_buf(out_tmp);
  }
  return 0;
}
*/
import "C"
import (
	"errors"
	"image"
)

type MorphologyMode uint8

const (
	MorphologyOpen MorphologyMode = iota + 1
	MorphologyClose
	MorphologyGradient
)

var (
	ErrMorphology = errors.New("morphology cgo call error")
)

func Morphology(img *image.RGBA, mode MorphologyMode, size int, count int) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libmorphology(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.uchar(mode),
		C.int(size),
		C.int(count),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrMorphology
	}
	return out, nil
}
