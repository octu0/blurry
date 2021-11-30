package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lflipV_osx -lflipH_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lflipV_linux -lflipH_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libflipV_osx.h"
#include "libflipH_osx.h"
#elif __linux__
#include "libflipV_linux.h"
#include "libflipH_linux.h"
#endif

#include "buffer.h"

int call_flip(
  uint8_t mode,
  halide_buffer_t *src,
  int32_t width, int32_t height,
  halide_buffer_t *out
) {
  if(mode == 1) {
    return flipV(src, width, height, out);
  }
  if(mode == 2) {
    return flipH(src, width, height, out);
  }
  return 1; // not found
}

int libflip(
  unsigned char *src,
  int32_t width,
  int32_t height,
  uint8_t mode,
  unsigned char *out
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = call_flip(mode, in_rgba_buf, width, height, out_rgba_buf);
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

type FlipMode uint8

const (
	FlipVertical FlipMode = iota + 1
	FlipHorizon
)

var (
	ErrFlip = errors.New("flip cgo call error")
)

func Flip(img *image.RGBA, mode FlipMode) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libflip(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.uchar(mode),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrFlip
	}
	return out, nil
}
