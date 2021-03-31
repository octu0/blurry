package blurry

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo darwin LDFLAGS: -L${SRCDIR} -lruntime_osx -lrotate_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR} -lruntime_linux -lrotate_linux -ldl -lm
#include <stdlib.h>

#include "bridge.h"
#ifdef __APPLE__
#include "librotate_osx.h"
#elif __linux__
#include "librotate_linux.h"
#endif

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

  int ret = rotate(in_rgba_buf, width, height, rotation, out_rgba_buf);
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
