package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lhighpass_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lhighpass_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libhighpass_osx.h"
#elif __linux__
#include "libhighpass_linux.h"
#endif

#include "buffer.h"

int libhighpass(unsigned char *src, int32_t width, int32_t height, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = highpass(in_rgba_buf, width, height, out_rgba_buf);
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
	ErrHighpass = errors.New("highpass cgo call error")
)

func Highpass(img *image.RGBA) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libhighpass(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrHighpass
	}
	return out, nil
}
