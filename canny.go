package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lcanny_osx -lcanny_dilate_osx -lcanny_morphology_open_osx -lcanny_morphology_close_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lcanny_linux -lcanny_dilate_linux -lcanny_morphology_open_linux -lcanny_morphology_close_linux -ldl -lm
#include <stdlib.h>

#ifdef __APPLE__
#include "libcanny_osx.h"
#include "libcanny_dilate_osx.h"
#include "libcanny_morphology_open_osx.h"
#include "libcanny_morphology_close_osx.h"
#elif __linux__
#include "libcanny_linux.h"
#include "libcanny_dilate_linux.h"
#include "libcanny_morphology_open_linux.h"
#include "libcanny_morphology_close_linux.h"
#endif

#include "buffer.h"

int call_canny(
  unsigned char mode,
  halide_buffer_t *in,
  int32_t width, int32_t height,
  int32_t tmax, int32_t tmin,
  int32_t msize,
  int32_t dsize,
  halide_buffer_t *out
) {
  if(0 < dsize) {
    if(mode == 0) {
      return canny_dilate(in, width, height, tmax, tmin, dsize, out);
    }
    if(mode == 1) {
      return canny_morphology_open(in, width, height, tmax, tmin, msize, dsize, out);
    }
    if(mode == 1) {
      return canny_morphology_close(in, width, height, tmax, tmin, msize, dsize, out);
    }
  }
  return canny(in, width, height, tmax, tmin, out);
}

int libcanny(
  unsigned char *src, int32_t width, int32_t height,
  int32_t threshold_max, int32_t threshold_min,
  unsigned char mode,
  int32_t morphology_size,
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

  int ret = call_canny(mode,
    in_rgba_buf, width, height,
    threshold_max, threshold_min,
    morphology_size,
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

const (
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
	mode CannyMorphologyMode,
	morphSize int,
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
