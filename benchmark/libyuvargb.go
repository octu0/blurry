package benchmark

/*
#cgo CFLAGS: -I${SRCDIR}/include/libyuv
#cgo LDFLAGS: -L${SRCDIR}/lib/libyuv -lyuv
#include "convert_argb.h"
#include "convert_from_argb.h"
*/
import "C"
import (
	"fmt"
	"image"
)

//int RGBAToARGB(const uint8_t* src_rgba,
//               int src_stride_rgba,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int width,
//               int height);
func RGBAToARGB(src *image.RGBA) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Max.X, b.Max.Y
	dst := image.NewRGBA(image.Rect(0, 0, width, height))

	stride := width * 4
	ret := C.RGBAToARGB(
		(*C.uchar)(&src.Pix[0]),
		C.int(stride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(stride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call RGBAToARGB:%d", ret)
	}
	return dst, nil
}

//int ARGBToRGBA(const uint8_t* src_rgba,
//               int src_stride_rgba,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int width,
//               int height);
func ARGBToRGBA(src *image.RGBA) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Max.X, b.Max.Y
	dst := image.NewRGBA(image.Rect(0, 0, width, height))

	stride := width * 4
	ret := C.ARGBToRGBA(
		(*C.uchar)(&src.Pix[0]),
		C.int(stride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(stride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBToRGBA:%d", ret)
	}
	return dst, nil
}
