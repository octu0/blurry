package blurry

/*
#cgo CFLAGS: -I.
#cgo darwin LDFLAGS: -L. -lruntime_osx -lcloneimg_osx -ldl -lm
#cgo linux  LDFLAGS: -L. -lruntime_linux -lcloneimg_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libcloneimg_osx.h"
#elif __linux__
#include "libcloneimg_linux.h"
#endif

#include "buffer.h"

int libcloneimg(unsigned char *src, int32_t width, int32_t height, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = cloneimg(in_rgba_buf, width, height, out_rgba_buf);
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
	ErrClone = errors.New("clone cgo call error")
)

func Clone(img *image.RGBA) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libcloneimg(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrClone
	}
	return out, nil
}
