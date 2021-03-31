package blurry

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo darwin LDFLAGS: -L${SRCDIR} -lruntime_osx -lsobel_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR} -lruntime_linux -lsobel_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libsobel_osx.h"
#elif __linux__
#include "libsobel_linux.h"
#endif

int libsobel(unsigned char *src, int32_t width, int32_t height, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = sobel(in_rgba_buf, width, height, out_rgba_buf);
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
	ErrSobel = errors.New("sobel cgo call error")
)

func Sobel(img *image.RGBA) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libsobel(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrSobel
	}
	return out, nil
}
