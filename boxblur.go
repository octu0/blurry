package blurry

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo darwin LDFLAGS: -L${SRCDIR} -lruntime_osx -lboxblur_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR} -lruntime_linux -lboxblur_linux -ldl -lm
#include <stdlib.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libboxblur_osx.h"
#elif __linux__
#include "libboxblur_linux.h"
#endif

int libboxblur(unsigned char *src, int32_t width, int32_t height, int32_t size, unsigned char *out) {
  halide_buffer_t in_rgba_buf = {0};
  halide_buffer_t out_rgba_buf = {0};

  bind_rgba_buf(&in_rgba_buf, src, width, height);
  bind_rgba_buf(&out_rgba_buf, out, width, height);

  int ret = boxblur(&in_rgba_buf, width, height, size, &out_rgba_buf);
  free_buf(&in_rgba_buf);
  free_buf(&out_rgba_buf);
  return ret;
}
*/
import "C"
import (
	"errors"
	"image"
)

var (
	ErrBoxblur = errors.New("boxblur cgo call error")
)

func Boxblur(img *image.RGBA, size int) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetNRGBA(width, height)

	ret := C.libboxblur(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.int(size),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrBoxblur
	}
	return out, nil
}
