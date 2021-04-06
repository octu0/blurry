package blurry

/*
#cgo CFLAGS: -I${SRCDIR}
#cgo darwin LDFLAGS: -L${SRCDIR} -lruntime_osx -lcanny_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR} -lruntime_linux -lcanny_linux -ldl -lm
#include <stdlib.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libcanny_osx.h"
#elif __linux__
#include "libcanny_linux.h"
#endif

int libcanny(
  unsigned char *src, int32_t width, int32_t height,
  int32_t threshold_max, int32_t threshold_min,
  unsigned char mode, int32_t size,
  int32_t dilate_size,
  unsigned char *out
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = canny(
    in_rgba_buf, width, height,
    threshold_max, threshold_min,
    mode, size,
    dilate_size,
    out_rgba_buf
  );
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

type CannyMorphologyMode uint8
const(
  CannyMorphologyNone CannyMorphologyMode = iota
  CannyMorphologyOpen
  CannyMorphologyClose
)

var (
	ErrCanny = errors.New("canny cgo call error")
)

func Canny(img *image.RGBA, thresholdMax, thresholdMin int) (*image.RGBA, error) {
	return CannyWithDilate(img, thresholdMax, thresholdMin, 0)
}

func CannyWithDilate(img *image.RGBA, thresholdMax, thresholdMin int, dilateSize int) (*image.RGBA, error) {
  return MorphologyCannyWithDilate(img, thresholdMax, thresholdMin, CannyMorphologyNone, 0, dilateSize)
}

func MorphologyCanny(img *image.RGBA, thresholdMax, thresholdMin int, mode CannyMorphologyMode, morphSize int) (*image.RGBA, error) {
  return MorphologyCannyWithDilate(img, thresholdMax, thresholdMin, mode, morphSize, 0)
}

func MorphologyCannyWithDilate(
  img *image.RGBA,
  thresholdMax, thresholdMin int,
  mode CannyMorphologyMode, morphSize int,
  dilateSize int,
) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libcanny(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.int(thresholdMax),
		C.int(thresholdMin),
    C.uchar(mode),
    C.int(morphSize),
		C.int(dilateSize),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrCanny
	}
	return out, nil
}
