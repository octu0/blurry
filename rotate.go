package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lrotate0_osx -lrotate90_osx -lrotate180_osx -lrotate270_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lrotate0_linux -lrotate90_linux -lrotate180_linux -lrotate270_linux -ldl -lm
#include <stdlib.h>

#include "bridge.h"
#ifdef __APPLE__
#include "librotate0_osx.h"
#include "librotate90_osx.h"
#include "librotate180_osx.h"
#include "librotate270_osx.h"
#elif __linux__
#include "librotate0_linux.h"
#include "librotate90_linux.h"
#include "librotate180_linux.h"
#include "librotate270_linux.h"
#endif

int call_rotate(
  int16_t rotation,
  halide_buffer_t *in,
  int32_t width, int32_t height,
  halide_buffer_t *out
) {
  if(rotation == 0) {
    return rotate0(in, width, height, out);
  }
  if(rotation == 90) {
    return rotate90(in, width, height, out);
  }
  if(rotation == 180) {
    return rotate180(in, width, height, out);
  }
  if(rotation == 270) {
    return rotate270(in, width, height, out);
  }
  return 1;
}

int librotate(unsigned char *src, int32_t width, int32_t height, int16_t rotation, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }

  halide_buffer_t *out_rgba_buf = NULL;
  if(rotation == 0 || rotation == 180) {
    out_rgba_buf = create_rgba_buffer(out, width, height);
  } else {
    out_rgba_buf = create_rgba_buffer(out, height, width);
  }
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = call_rotate(rotation, in_rgba_buf, width, height, out_rgba_buf);
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

type RotationMode int16

const (
	Rotate0   RotationMode = 0   // No rotation
	Rotate90  RotationMode = 90  // Rotate 90 degrees clockwise
	Rotate180 RotationMode = 180 // Rotate 180 degrees
	Rotate270 RotationMode = 270 // Rotate 270 degrees clockwise
)

var (
	ErrRotate = errors.New("rotate cgo call error")
)

func Rotate(img *image.RGBA, mode RotationMode) (*image.RGBA, error) {
	width, height := wh(img)
	var out *image.RGBA
	switch mode {
	case Rotate0, Rotate180:
		out = GetRGBA(width, height)
	case Rotate90, Rotate270:
		out = GetRGBA(height, width)
	default:
		out = GetRGBA(width, height)
	}

	ret := C.librotate(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.short(mode),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrRotate
	}
	return out, nil
}
