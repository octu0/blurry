package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lcrop_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lcrop_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libcrop_osx.h"
#elif __linux__
#include "libcrop_linux.h"
#endif

#include "buffer.h"

int libcrop(
  unsigned char *src, int32_t width, int32_t height,
  int32_t x, int32_t y, int32_t crop_width, int32_t crop_height,
  unsigned char *out
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, crop_width, crop_height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = crop(in_rgba_buf, width, height, x, y, crop_width, crop_height, out_rgba_buf);
  free_buf(in_rgba_buf);
  free_buf(out_rgba_buf);
  return ret;
}
*/
import "C"
import (
	"image"
	"unsafe"

	"github.com/pkg/errors"
)

var (
	ErrCrop = errors.New("crop cgo call error")
)

func Crop(img *image.RGBA, pt image.Point, cropWidth, cropHeight int) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(cropWidth, cropHeight)

	ret := C.libcrop(
		(*C.uchar)(unsafe.Pointer(&img.Pix[0])),
		C.int(width),
		C.int(height),
		C.int(pt.X),
		C.int(pt.Y),
		C.int(cropWidth),
		C.int(cropHeight),
		(*C.uchar)(unsafe.Pointer(&out.Pix[0])),
	)
	if int(ret) != 0 {
		PutRGBA(out)
		return nil, errors.WithStack(ErrCrop)
	}
	return out, nil
}
