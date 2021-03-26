package blurry

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo darwin LDFLAGS: -L${SRCDIR} -lruntime_osx -lcontrast_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR} -lruntime_linux -lcontrast_linux -ldl -lm
#include <stdlib.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libcontrast_osx.h"
#elif __linux__
#include "libcontrast_linux.h"
#endif

int libcontrast(unsigned char *src, int32_t width, int32_t height, float factor, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    return 1;
  }

  int ret = contrast(in_rgba_buf, width, height, factor, out_rgba_buf);
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

var (
	ErrContrast = errors.New("contrast cgo call error")
)

func Contrast(img *image.RGBA, factor float64) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libcontrast(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.float(factor),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrContrast
	}
	return out, nil
}
