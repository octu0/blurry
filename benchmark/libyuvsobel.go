package benchmark

/*
#cgo CFLAGS: -I${SRCDIR}/include/libyuv
#cgo LDFLAGS: -L${SRCDIR}/lib/libyuv -lyuv
#include "planar_functions.h"
*/
import "C"
import (
	"fmt"
	"image"
)

//int ARGBSobel(const uint8_t* src_argb,
//              int src_stride_argb,
//              uint8_t* dst_argb,
//              int dst_stride_argb,
//              int width,
//              int height);
func ARGBSobel(src *image.RGBA) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Max.X, b.Max.Y
	dst := image.NewRGBA(image.Rect(0, 0, width, height))

	stride := width * 4
	ret := C.ARGBSobel(
		(*C.uchar)(&src.Pix[0]),
		C.int(stride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(stride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBSobel:%d", ret)
	}
	return dst, nil
}
