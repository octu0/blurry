package blurry

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo darwin LDFLAGS: -L${SRCDIR} -lruntime_osx -lblockmozaic_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR} -lruntime_linux -lblockmozaic_linux -ldl -lm
#include <stdlib.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libblockmozaic_osx.h"
#elif __linux__
#include "libblockmozaic_linux.h"
#endif

int libblockmozaic(unsigned char *src, int32_t width, int32_t height, int32_t block, unsigned char *out) {
  halide_buffer_t in_rgba_buf = {0};
  halide_buffer_t out_rgba_buf = {0};

  bind_rgba_buf(&in_rgba_buf, src, width, height);
  bind_rgba_buf(&out_rgba_buf, out, width, height);

  int ret = blockmozaic(&in_rgba_buf, width, height, block, &out_rgba_buf);
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
	ErrBlockmozaic = errors.New("contrast cgo call error")
)

func Blockmozaic(img *image.RGBA, block int) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetNRGBA(width, height)

	ret := C.libblockmozaic(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.int(block),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrBlockmozaic
	}
	return out, nil
}
