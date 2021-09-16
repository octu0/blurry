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

//int ARGBAdd(const uint8_t* src_argb0,
//            int src_stride_argb0,
//            const uint8_t* src_argb1,
//            int src_stride_argb1,
//            uint8_t* dst_argb,
//            int dst_stride_argb,
//            int width,
//            int height);
func ARGBAdd(src0, src1 *image.RGBA) (*image.RGBA, error) {
	b0 := src0.Bounds()
	b1 := src1.Bounds()
	width0, height0 := b0.Max.X, b0.Max.Y
	width1 := b1.Max.X
	dst := image.NewRGBA(image.Rect(0, 0, width0, height0))

	stride0 := width0 * 4
	stride1 := width1 * 4
	ret := C.ARGBAdd(
		(*C.uchar)(&src0.Pix[0]),
		C.int(stride0),
		(*C.uchar)(&src1.Pix[0]),
		C.int(stride1),
		(*C.uchar)(&dst.Pix[0]),
		C.int(stride0),
		C.int(width0),
		C.int(height0),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBAdd:%d", ret)
	}
	return dst, nil
}

//int ARGBSubtract(const uint8_t* src_argb0,
//                 int src_stride_argb0,
//                 const uint8_t* src_argb1,
//                 int src_stride_argb1,
//                 uint8_t* dst_argb,
//                 int dst_stride_argb,
//                 int width,
//                 int height);
func ARGBSubtract(src0, src1 *image.RGBA) (*image.RGBA, error) {
	b0 := src0.Bounds()
	b1 := src1.Bounds()
	width0, height0 := b0.Max.X, b0.Max.Y
	width1 := b1.Max.X
	dst := image.NewRGBA(image.Rect(0, 0, width0, height0))

	stride0 := width0 * 4
	stride1 := width1 * 4
	ret := C.ARGBSubtract(
		(*C.uchar)(&src0.Pix[0]),
		C.int(stride0),
		(*C.uchar)(&src1.Pix[0]),
		C.int(stride1),
		(*C.uchar)(&dst.Pix[0]),
		C.int(stride0),
		C.int(width0),
		C.int(height0),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBSubtract:%d", ret)
	}
	return dst, nil
}
