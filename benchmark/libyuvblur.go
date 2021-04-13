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
	"unsafe"
)

//int ARGBBlur(const uint8_t* src_argb,
//             int src_stride_argb,
//             uint8_t* dst_argb,
//             int dst_stride_argb,
//             int32_t* dst_cumsum,
//             int dst_stride32_cumsum,
//             int width,
//             int height,
//             int radius);
//
func ARGBBlur(src *image.RGBA, radius int) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Max.X, b.Max.Y
	dst := image.NewRGBA(image.Rect(0, 0, width, height))

	cumsum := make([]int, width*(height+1)*16)
	stride := width * 4
	ret := C.ARGBBlur(
		(*C.uchar)(&src.Pix[0]),
		C.int(stride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(stride),
		(*C.int)(unsafe.Pointer(&cumsum[0])),
		C.int(stride),
		C.int(width),
		C.int(height),
		C.int(radius),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBBlur:%d", ret)
	}
	return dst, nil
}
