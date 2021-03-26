package blurry

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo darwin LDFLAGS: -L${SRCDIR} -lruntime_osx -ledge_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR} -lruntime_linux -ledge_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libedge_osx.h"
#elif __linux__
#include "libedge_linux.h"
#endif

int libedge(unsigned char *src, int32_t width, int32_t height, unsigned char *out) {
  halide_buffer_t in_rgba_buf = {0};
  halide_buffer_t out_rgba_buf = {0};

  bind_rgba_buf(&in_rgba_buf, src, width, height);
  bind_rgba_buf(&out_rgba_buf, out, width, height);

  int ret = edge(&in_rgba_buf, width, height, &out_rgba_buf);
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
	ErrEdge = errors.New("edge cgo call error")
)

func Edge(img *image.RGBA) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libedge(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrEdge
	}
	return out, nil
}