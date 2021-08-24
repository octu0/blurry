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

type convertCFunc func(
	src *C.uchar,
	src_stride C.int,
	dst *C.uchar,
	dst_stride C.int,
	width C.int,
	height C.int,
) C.int

func convertFrom(src *image.RGBA, fn convertCFunc, name string) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Max.X, b.Max.Y
	dst := image.NewRGBA(image.Rect(0, 0, width, height))

	stride := width * 4
	ret := fn(
		(*C.uchar)(&src.Pix[0]),
		C.int(stride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(stride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call %s(%d)", name, ret)
	}
	return dst, nil
}

//int ARGBToABGR(const uint8_t* src_argb,
//               int src_stride_argb,
//               uint8_t* dst_abgr,
//               int dst_stride_abgr,
//               int width,
//               int height);
//
func ARGBToABGR(src *image.RGBA) (*image.RGBA, error) {
	return convertFrom(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
		return C.ARGBToABGR(s, srcStride, d, dstStride, width, height)
	}, "ARGBToABGR")
}

//int ABGRToARGB(const uint8_t* src_abgr,
//               int src_stride_abgr,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int width,
//               int height);
func ABGRToARGB(src *image.RGBA) (*image.RGBA, error) {
	return convertFrom(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
		return C.ABGRToARGB(s, srcStride, d, dstStride, width, height)
	}, "ABGRToARGB")
}

//int ARGBToRGBA(const uint8_t* src_argb,
//               int src_stride_argb,
//               uint8_t* dst_rgba,
//               int dst_stride_rgba,
//               int width,
//               int height);
func ARGBToRGBA(src *image.RGBA) (*image.RGBA, error) {
	return convertFrom(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
		return C.ARGBToRGBA(s, srcStride, d, dstStride, width, height)
	}, "ARGBToRGBA")
}

//int RGBAToARGB(const uint8_t* src_rgba,
//               int src_stride_rgba,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int width,
//               int height);
func RGBAToARGB(src *image.RGBA) (*image.RGBA, error) {
	return convertFrom(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
		return C.RGBAToARGB(s, srcStride, d, dstStride, width, height)
	}, "RGBAToARGB")
}

//int BGRAToARGB(const uint8_t* src_bgra,
//               int src_stride_bgra,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int width,
//               int height);
func BGRAToARGB(src *image.RGBA) (*image.RGBA, error) {
	return convertFrom(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
		return C.BGRAToARGB(s, srcStride, d, dstStride, width, height)
	}, "BGRAToARGB")
}

//int ARGBToBGRA(const uint8_t* src_argb,
//               int src_stride_argb,
//               uint8_t* dst_bgra,
//               int dst_stride_bgra,
//               int width,
//               int height);
func ARGBToBGRA(src *image.RGBA) (*image.RGBA, error) {
	return convertFrom(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
		return C.ARGBToBGRA(s, srcStride, d, dstStride, width, height)
	}, "ARGBToBGRA")
}
