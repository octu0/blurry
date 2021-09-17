package benchmark

/*
#cgo CFLAGS: -I${SRCDIR}/include/libyuv
#cgo LDFLAGS: -L${SRCDIR}/lib/libyuv -lyuv
#include "convert.h"
#include "convert_argb.h"
#include "convert_from_argb.h"
*/
import "C"
import (
	"fmt"
	"image"
)

type convertCFuncRGBA func(
	src *C.uchar,
	src_stride C.int,
	dst *C.uchar,
	dst_stride C.int,
	width C.int,
	height C.int,
) C.int

func convertRGBA(src *image.RGBA, fn convertCFuncRGBA, name string) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Dx(), b.Dy()
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
	return convertRGBA(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
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
	return convertRGBA(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
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
	return convertRGBA(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
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
	return convertRGBA(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
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
	return convertRGBA(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
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
	return convertRGBA(src, func(s *C.uchar, srcStride C.int, d *C.uchar, dstStride C.int, width, height C.int) C.int {
		return C.ARGBToBGRA(s, srcStride, d, dstStride, width, height)
	}, "ARGBToBGRA")
}

type convertCFuncI420 func(
	src *C.uchar,
	src_stride C.int,
	dst_y *C.uchar,
	dst_stride_y C.int,
	dst_u *C.uchar,
	dst_stride_u C.int,
	dst_v *C.uchar,
	dst_stride_v C.int,
	width C.int,
	height C.int,
) C.int

func convertI420(src *image.RGBA, fn convertCFuncI420, name string) (*image.YCbCr, error) {
	sample := image.YCbCrSubsampleRatio420
	rect := src.Bounds()
	width, height := rect.Dx(), rect.Dy()

	yStride := width
	uvStride := width / 2

	ySize := width * height
	uvSize := ((width + 1) / 2) * ((height + 1) / 2)

	dst := &image.YCbCr{
		Y:              make([]byte, ySize),
		Cb:             make([]byte, uvSize),
		Cr:             make([]byte, uvSize),
		YStride:        yStride,
		CStride:        uvStride,
		Rect:           rect,
		SubsampleRatio: sample,
	}

	ret := fn(
		(*C.uchar)(&src.Pix[0]),
		C.int(width*4),
		(*C.uchar)(&dst.Y[0]),
		C.int(dst.YStride),
		(*C.uchar)(&dst.Cb[0]),
		C.int(dst.CStride),
		(*C.uchar)(&dst.Cr[0]),
		C.int(dst.CStride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call %s(%d)", name, ret)
	}
	return dst, nil
}

//int ARGBToI420(const uint8_t* src_argb,
//               int src_stride_argb,
//               uint8_t* dst_y,
//               int dst_stride_y,
//               uint8_t* dst_u,
//               int dst_stride_u,
//               uint8_t* dst_v,
//               int dst_stride_v,
//               int width,
//               int height);
func ARGBToI420(src *image.RGBA) (*image.YCbCr, error) {
	return convertI420(src, func(
		s *C.uchar, stride C.int,
		y *C.uchar, yStride C.int,
		u *C.uchar, uStride C.int,
		v *C.uchar, vStride C.int,
		width C.int,
		height C.int,
	) C.int {
		return C.ARGBToI420(s, stride, y, yStride, u, uStride, v, vStride, width, height)
	}, "ARGBToI420")
}

//int ABGRToI420(const uint8_t* src_abgr,
//               int src_stride_abgr,
//               uint8_t* dst_y,
//               int dst_stride_y,
//               uint8_t* dst_u,
//               int dst_stride_u,
//               uint8_t* dst_v,
//               int dst_stride_v,
//               int width,
//               int height);
func ABGRToI420(src *image.RGBA) (*image.YCbCr, error) {
	return convertI420(src, func(
		s *C.uchar, stride C.int,
		y *C.uchar, yStride C.int,
		u *C.uchar, uStride C.int,
		v *C.uchar, vStride C.int,
		width C.int,
		height C.int,
	) C.int {
		return C.ABGRToI420(s, stride, y, yStride, u, uStride, v, vStride, width, height)
	}, "ABGRToI420")
}

//int RGBAToI420(const uint8_t* src_rgba,
//               int src_stride_rgba,
//               uint8_t* dst_y,
//               int dst_stride_y,
//               uint8_t* dst_u,
//               int dst_stride_u,
//               uint8_t* dst_v,
//               int dst_stride_v,
//               int width,
//               int height);
func RGBAToI420(src *image.RGBA) (*image.YCbCr, error) {
	return convertI420(src, func(
		s *C.uchar, stride C.int,
		y *C.uchar, yStride C.int,
		u *C.uchar, uStride C.int,
		v *C.uchar, vStride C.int,
		width C.int,
		height C.int,
	) C.int {
		return C.RGBAToI420(s, stride, y, yStride, u, uStride, v, vStride, width, height)
	}, "RGBAToI420")
}

//int BGRAToI420(const uint8_t* src_bgra,
//               int src_stride_bgra,
//               uint8_t* dst_y,
//               int dst_stride_y,
//               uint8_t* dst_u,
//               int dst_stride_u,
//               uint8_t* dst_v,
//               int dst_stride_v,
//               int width,
//               int height);
func BGRAToI420(src *image.RGBA) (*image.YCbCr, error) {
	return convertI420(src, func(
		s *C.uchar, stride C.int,
		y *C.uchar, yStride C.int,
		u *C.uchar, uStride C.int,
		v *C.uchar, vStride C.int,
		width C.int,
		height C.int,
	) C.int {
		return C.BGRAToI420(s, stride, y, yStride, u, uStride, v, vStride, width, height)
	}, "BGRAToI420")
}

//int ARGBToI444(const uint8_t* src_argb,
//               int src_stride_argb,
//               uint8_t* dst_y,
//               int dst_stride_y,
//               uint8_t* dst_u,
//               int dst_stride_u,
//               uint8_t* dst_v,
//               int dst_stride_v,
//               int width,
//               int height);
func ARGBToI444(src *image.RGBA) (*image.YCbCr, error) {
	sample := image.YCbCrSubsampleRatio420
	rect := src.Bounds()
	width, height := rect.Dx(), rect.Dy()

	yStride := width
	uvStride := width

	ySize := width * height
	uvSize := width * height

	dst := &image.YCbCr{
		Y:              make([]byte, ySize),
		Cb:             make([]byte, uvSize),
		Cr:             make([]byte, uvSize),
		YStride:        yStride,
		CStride:        uvStride,
		Rect:           rect,
		SubsampleRatio: sample,
	}
	ret := C.ARGBToI444(
		(*C.uchar)(&src.Pix[0]),
		C.int(width*4),
		(*C.uchar)(&dst.Y[0]),
		C.int(dst.YStride),
		(*C.uchar)(&dst.Cb[0]),
		C.int(dst.CStride),
		(*C.uchar)(&dst.Cr[0]),
		C.int(dst.CStride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call %s(%d)", "ARGBToI444", ret)
	}
	return dst, nil
}

//int I420ToARGB(const uint8_t* src_y,
//               int src_stride_y,
//               const uint8_t* src_u,
//               int src_stride_u,
//               const uint8_t* src_v,
//               int src_stride_v,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int width,
//               int height);
func I420ToARGB(src *image.YCbCr) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Dx(), b.Dy()
	dst := image.NewRGBA(image.Rect(0, 0, width, height))

	ret := C.I420ToARGB(
		(*C.uchar)(&src.Y[0]),
		C.int(src.YStride),
		(*C.uchar)(&src.Cb[0]),
		C.int(src.CStride),
		(*C.uchar)(&src.Cr[0]),
		C.int(src.CStride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(dst.Stride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call %s(%d)", "I420ToARGB", ret)
	}
	return dst, nil
}

//int I444ToARGB(const uint8_t* src_y,
//               int src_stride_y,
//               const uint8_t* src_u,
//               int src_stride_u,
//               const uint8_t* src_v,
//               int src_stride_v,
//               uint8_t* dst_argb,
//               int dst_stride_argb,
//               int width,
//               int height);
func I444ToARGB(src *image.YCbCr) (*image.RGBA, error) {
	b := src.Bounds()
	width, height := b.Dx(), b.Dy()
	dst := image.NewRGBA(image.Rect(0, 0, width, height))

	ret := C.I444ToARGB(
		(*C.uchar)(&src.Y[0]),
		C.int(src.YStride),
		(*C.uchar)(&src.Cb[0]),
		C.int(src.CStride),
		(*C.uchar)(&src.Cr[0]),
		C.int(src.CStride),
		(*C.uchar)(&dst.Pix[0]),
		C.int(dst.Stride),
		C.int(width),
		C.int(height),
	)
	if ret != 0 {
		return nil, fmt.Errorf("failed to call %s(%d)", "I420ToARGB", ret)
	}
	return dst, nil
}
