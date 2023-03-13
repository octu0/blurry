package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -ledgedetect_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -ledgedetect_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libedgedetect_osx.h"
#elif __linux__
#include "libedgedetect_linux.h"
#endif

#include "buffer.h"

int libedge(unsigned char *src, int32_t width, int32_t height, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = edgedetect(in_rgba_buf, width, height, out_rgba_buf);
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
	ErrEdge = errors.New("edge cgo call error")
)

func Edge(img *image.RGBA) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libedge(
		(*C.uchar)(unsafe.Pointer(&img.Pix[0])),
		C.int(width),
		C.int(height),
		(*C.uchar)(unsafe.Pointer(&out.Pix[0])),
	)
	if int(ret) != 0 {
		PutRGBA(out)
		return nil, errors.WithStack(ErrEdge)
	}
	return out, nil
}
