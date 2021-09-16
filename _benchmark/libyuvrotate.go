package benchmark

/*
#cgo CFLAGS: -I${SRCDIR}/include/libyuv
#cgo LDFLAGS: -L${SRCDIR}/lib/libyuv -lyuv
#include "rotate.h"
#include "rotate_argb.h"
*/
import "C"
import (
	"fmt"
	"image"
)

type RotationMode uint16

const (
	Rotate0   RotationMode = 0
	Rotate90  RotationMode = 90
	Rotate180 RotationMode = 180
	Rotate270 RotationMode = 270
)

//int ARGBRotate(const uint8_t* src_argb,
//               int src_stride_argb,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int src_width,
//               int src_height,
//               enum RotationMode mode);
func ARGBRotate(src *image.RGBA, mode RotationMode) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Max.X, b.Max.Y
	srcStride := width * 4
	dstStride := width * 4

	var dst *image.RGBA
	switch mode {
	case Rotate0, Rotate180:
		dst = image.NewRGBA(image.Rect(0, 0, width, height))
	case Rotate90, Rotate270:
		dstStride = height * 4
		dst = image.NewRGBA(image.Rect(0, 0, height, width))
	}

	ret := C.ARGBRotate(
		(*C.uchar)(&src.Pix[0]),
		C.int(srcStride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(dstStride),
		C.int(width),
		C.int(height),
		C.enum_RotationMode(uint16(mode)),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call ARGBRotate:%d", ret)
	}
	return dst, nil
}
