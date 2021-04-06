package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -ldilation_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -ldilation_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libdilation_osx.h"
#elif __linux__
#include "libdilation_linux.h"
#endif

int libdilation(unsigned char *src, int32_t width, int32_t height, int32_t size, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = dilation(in_rgba_buf, width, height, size, out_rgba_buf);
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
	ErrDilation = errors.New("dilation cgo call error")
)

func Dilation(img *image.RGBA, size int) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libdilation(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.int(size),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrDilation
	}
	return out, nil
}
