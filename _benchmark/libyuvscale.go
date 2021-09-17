package benchmark

/*
#cgo CFLAGS: -I${SRCDIR}/include/libyuv
#cgo LDFLAGS: -L${SRCDIR}/lib/libyuv -lyuv
#include <stdint.h>
#include "scale_argb.h"
int callARGBScale(
  const uint8_t* src_argb,
  int src_stride_argb,
  int src_width,
  int src_height,
  uint8_t* dst_argb,
  int dst_stride_argb,
  int dst_width,
  int dst_height,
  unsigned char f
) {
  enum FilterMode mode = kFilterNone;
  if(f == 0) {
    mode = kFilterNone;
  }
  if(f == 1) {
    mode = kFilterLinear;
  }
  if(f == 2) {
    mode = kFilterBilinear;
  }
  if(f == 3) {
    mode = kFilterBox;
  }
  return ARGBScale(src_argb, src_stride_argb, src_width, src_height, dst_argb, dst_stride_argb, dst_width, dst_height, mode);
}
*/
import "C"
import (
	"fmt"
	"image"
)

type FilterMode uint8

const (
	FilterNone FilterMode = iota
	FilterLinear
	FilterBilinear
	FilterBox
)

//int ARGBScale(const uint8_t* src_argb,
//              int src_stride_argb,
//              int src_width,
//              int src_height,
//              uint8_t* dst_argb,
//              int dst_stride_argb,
//              int dst_width,
//              int dst_height,
//              enum FilterMode filtering);
func ARGBScale(src *image.RGBA, width, height int, mode FilterMode) (*image.RGBA, error) {
	b := src.Bounds()
	srcW, srcH := b.Dx(), b.Dy()

	dst := image.NewRGBA(image.Rect(0, 0, width, height))
	ret := C.callARGBScale(
		(*C.uchar)(&src.Pix[0]),
		C.int(src.Stride),
		C.int(srcW),
		C.int(srcH),
		(*C.uchar)(&dst.Pix[0]),
		C.int(dst.Stride),
		C.int(width),
		C.int(height),
		C.uchar(mode),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBScale:%d", ret)
	}
	return dst, nil
}

//int ARGBScaleClip(const uint8_t* src_argb,
//                  int src_stride_argb,
//                  int src_width,
//                  int src_height,
//                  uint8_t* dst_argb,
//                  int dst_stride_argb,
//                  int dst_width,
//                  int dst_height,
//                  int clip_x,
//                  int clip_y,
//                  int clip_width,
//                  int clip_height,
//                  enum FilterMode filtering);
func ARGBScaleClip(src *image.RGBA, x, y int, clip_width, clip_height int) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Dx(), b.Dy()

	dst := image.NewRGBA(image.Rect(0, 0, width, height))
	ret := C.ARGBScaleClip(
		(*C.uchar)(&src.Pix[0]),
		C.int(src.Stride),
		C.int(width),
		C.int(height),
		(*C.uchar)(&dst.Pix[0]),
		C.int(dst.Stride),
		C.int(width),
		C.int(height),
		C.int(x),
		C.int(y),
		C.int(clip_width),
		C.int(clip_height),
		C.kFilterNone,
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBScaleClip:%d", ret)
	}
	return dst, nil
}
