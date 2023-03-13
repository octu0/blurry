package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -ldl -lm
#cgo darwin LDFLAGS: -lconvert_from_yuv_420_osx
#cgo darwin LDFLAGS: -lconvert_from_yuv_444_osx
#cgo darwin LDFLAGS: -lconvert_to_yuv_420_osx
#cgo darwin LDFLAGS: -lconvert_to_yuv_444_osx
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -ldl -lm
#cgo linux  LDFLAGS: -lconvert_from_yuv_420_linux
#cgo linux  LDFLAGS: -lconvert_from_yuv_444_linux
#cgo linux  LDFLAGS: -lconvert_to_yuv_420_linux
#cgo linux  LDFLAGS: -lconvert_to_yuv_444_linux
#include <stdlib.h>

#ifdef __APPLE__
#include "libconvert_from_yuv_420_osx.h"
#include "libconvert_from_yuv_444_osx.h"
#include "libconvert_to_yuv_420_osx.h"
#include "libconvert_to_yuv_444_osx.h"
#elif __linux__
#include "libconvert_from_yuv_420_linux.h"
#include "libconvert_from_yuv_444_linux.h"
#include "libconvert_to_yuv_420_linux.h"
#include "libconvert_to_yuv_444_linux.h"
#endif

#include "buffer.h"

int call_convert_from_yuv(
  unsigned char chrs,
  halide_buffer_t *in_y_buf,
  halide_buffer_t *in_u_buf,
  halide_buffer_t *in_v_buf,
  int32_t width, int32_t height,
  halide_buffer_t *out_buf
) {
  if(1 == chrs) {
    return convert_from_yuv_420(in_y_buf, in_u_buf, in_v_buf, width, height, out_buf);
  }
  if(2 == chrs) {
    return convert_from_yuv_444(in_y_buf, in_u_buf, in_v_buf, width, height, out_buf);
  }
  return 1;
}

halide_buffer_t *create_yuv_y_plane(
  unsigned char chrs,
  unsigned char *src, int32_t stride,
  int32_t width, int32_t height
) {
  return create_yuv_plane_buffer(src, stride, width, height);
}

halide_buffer_t *create_yuv_uv_plane(
  unsigned char chrs,
  unsigned char *src, int32_t stride,
  int32_t width, int32_t height
) {
  if(1 == chrs) {
    return create_yuv_plane_buffer(src, stride, width / 2, height / 2);
  }
  if(2 == chrs) {
    return create_yuv_plane_buffer(src, stride, width, height);
  }
  return NULL;
}

int libconvert_from_yuv(
  unsigned char *src_y,
  unsigned char *src_u,
  unsigned char *src_v,
  int32_t y_stride,
  int32_t u_stride,
  int32_t v_stride,
  int32_t width, int32_t height,
  unsigned char chrs,
  unsigned char *out
) {
  halide_buffer_t *in_y_buf = create_yuv_y_plane(chrs, src_y, y_stride, width, height);
  if(in_y_buf == NULL){
    return 1;
  }

  halide_buffer_t *in_u_buf = create_yuv_uv_plane(chrs, src_u, u_stride, width, height);
  if(in_u_buf == NULL){
    free_buf(in_y_buf);
    return 1;
  }

  halide_buffer_t *in_v_buf = create_yuv_uv_plane(chrs, src_v, v_stride, width, height);
  if(in_v_buf == NULL){
    free_buf(in_y_buf);
    free_buf(in_u_buf);
    return 1;
  }

  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_y_buf);
    free_buf(in_u_buf);
    free_buf(in_v_buf);
    return 1;
  }

  int ret = call_convert_from_yuv(chrs, in_y_buf, in_u_buf, in_v_buf, width, height, out_rgba_buf);
  free_buf(in_y_buf);
  free_buf(in_u_buf);
  free_buf(in_v_buf);
  free_buf(out_rgba_buf);
  return ret;
}

int libconvert_to_yuv420(
  unsigned char *src,
  int32_t width, int32_t height,
  unsigned char *out_y,
  unsigned char *out_u,
  unsigned char *out_v
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_y_buf = create_uint8_array_buffer(out_y, width, height);
  if(out_y_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }
  halide_buffer_t *out_u_buf = create_uint8_array_buffer(out_u, width / 2, height / 2);
  if(out_u_buf == NULL){
    free_buf(in_rgba_buf);
    free_buf(out_y_buf);
    return 1;
  }
  halide_buffer_t *out_v_buf = create_uint8_array_buffer(out_v, width / 2, height / 2);
  if(out_v_buf == NULL){
    free_buf(in_rgba_buf);
    free_buf(out_y_buf);
    free_buf(out_u_buf);
    return 1;
  }

  int ret = convert_to_yuv_420(in_rgba_buf, width, height, out_y_buf, out_u_buf, out_v_buf);
  free_buf(in_rgba_buf);
  free_buf(out_y_buf);
  free_buf(out_u_buf);
  free_buf(out_v_buf);
  return ret;
}

int libconvert_to_yuv444(
  unsigned char *src,
  int32_t width, int32_t height,
  unsigned char *out_y,
  unsigned char *out_u,
  unsigned char *out_v
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_y_buf = create_uint8_array_buffer(out_y, width, height);
  if(out_y_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }
  halide_buffer_t *out_u_buf = create_uint8_array_buffer(out_u, width, height);
  if(out_u_buf == NULL){
    free_buf(in_rgba_buf);
    free_buf(out_y_buf);
    return 1;
  }
  halide_buffer_t *out_v_buf = create_uint8_array_buffer(out_v, width, height);
  if(out_v_buf == NULL){
    free_buf(in_rgba_buf);
    free_buf(out_y_buf);
    free_buf(out_u_buf);
    return 1;
  }

  int ret = convert_to_yuv_444(in_rgba_buf, width, height, out_y_buf, out_u_buf, out_v_buf);
  free_buf(in_rgba_buf);
  free_buf(out_y_buf);
  free_buf(out_u_buf);
  free_buf(out_v_buf);
  return ret;
}
*/
import "C"
import (
	"image"
	"unsafe"

	"github.com/pkg/errors"
)

var (
	ErrConvertFromYUV           = errors.New("convert_from cgo call error")
	ErrConvertToYUV             = errors.New("convert_to cgo call error")
	ErrYUVSubsampleRateMustI420 = errors.New("image.YCbCr.SubsampleRatio must be 420")
	ErrYUVSubsampleRateMustI444 = errors.New("image.YCbCr.SubsampleRatio must be 444")
)

func ConvertFromYUV(img *image.YCbCr, chrs ChromaSubsampling) (*image.RGBA, error) {
	width, height := whYCbCr(img)
	out := GetRGBA(width, height)

	ret := C.libconvert_from_yuv(
		(*C.uchar)(unsafe.Pointer(&img.Y[0])),
		(*C.uchar)(unsafe.Pointer(&img.Cb[0])),
		(*C.uchar)(unsafe.Pointer(&img.Cr[0])),
		C.int(img.YStride),
		C.int(img.CStride),
		C.int(img.CStride),
		C.int(width),
		C.int(height),
		C.uchar(chrs),
		(*C.uchar)(unsafe.Pointer(&out.Pix[0])),
	)
	if int(ret) != 0 {
		return nil, errors.WithStack(ErrConvertFromYUV)
	}
	return out, nil
}

func ConvertFromYUV420(img *image.YCbCr) (*image.RGBA, error) {
	if img.SubsampleRatio != image.YCbCrSubsampleRatio420 {
		return nil, errors.WithStack(ErrYUVSubsampleRateMustI420)
	}
	return ConvertFromYUV(img, ChromaSubsampling420)
}

func ConvertFromYUV444(img *image.YCbCr) (*image.RGBA, error) {
	if img.SubsampleRatio != image.YCbCrSubsampleRatio444 {
		return nil, errors.WithStack(ErrYUVSubsampleRateMustI444)
	}
	return ConvertFromYUV(img, ChromaSubsampling444)
}

func ConvertFromYUV420Plane(y, u, v []byte, strideY, strideU, strideV int, width, height int) (*image.RGBA, error) {
	img := &image.YCbCr{
		Y:              y,
		Cb:             u,
		Cr:             v,
		Rect:           image.Rect(0, 0, width, height),
		YStride:        strideY,
		CStride:        strideU,
		SubsampleRatio: image.YCbCrSubsampleRatio420,
	}
	return ConvertFromYUV(img, ChromaSubsampling420)
}

func ConvertFromYUV444Plane(y, u, v []byte, strideY, strideU, strideV int, width, height int) (*image.RGBA, error) {
	img := &image.YCbCr{
		Y:              y,
		Cb:             u,
		Cr:             v,
		Rect:           image.Rect(0, 0, width, height),
		YStride:        strideY,
		CStride:        strideU,
		SubsampleRatio: image.YCbCrSubsampleRatio444,
	}
	return ConvertFromYUV(img, ChromaSubsampling444)
}

func ConvertToYUV420(img *image.RGBA) (*image.YCbCr, error) {
	width, height := wh(img)
	uvWidth := width / 2
	uvHeight := height / 2
	ySize := width * height
	uSize := uvWidth * uvHeight
	vSize := uvWidth * uvHeight

	//
	// data layout
	// +-----------------+ width
	// | Y Y Y Y Y Y Y Y |
	// | Y Y Y Y Y Y Y Y |
	// | Y Y Y Y Y Y Y Y |
	// | Y Y Y Y Y Y Y Y |
	// +---------+-------+ height
	// | U U U U | 0 0 0 |
	// | U U U U | 0 0 0 |
	// +---------+-------+ uvHeight
	// | V V V V | 0 0 0 |
	// | V V V V | 0 0 0 |
	// +-----------------+
	//           uvWidth
	//

	yBuf := GetByteBuf(ySize)
	uBuf := GetByteBuf(uSize)
	vBuf := GetByteBuf(vSize)

	ret := C.libconvert_to_yuv420(
		(*C.uchar)(unsafe.Pointer(&img.Pix[0])),
		C.int(width),
		C.int(height),
		(*C.uchar)(unsafe.Pointer(&yBuf[0])),
		(*C.uchar)(unsafe.Pointer(&uBuf[0])),
		(*C.uchar)(unsafe.Pointer(&vBuf[0])),
	)
	if int(ret) != 0 {
		PutByteBuf(yBuf)
		PutByteBuf(uBuf)
		PutByteBuf(vBuf)
		return nil, errors.WithStack(ErrConvertToYUV)
	}

	out := &image.YCbCr{
		Y:              yBuf,
		Cb:             uBuf,
		Cr:             vBuf,
		YStride:        width,
		CStride:        uvWidth,
		SubsampleRatio: image.YCbCrSubsampleRatio420,
		Rect:           image.Rect(0, 0, width, height),
	}
	return out, nil
}

func ConvertToYUV444(img *image.RGBA) (*image.YCbCr, error) {
	width, height := wh(img)
	ySize := width * height
	uSize := width * height
	vSize := width * height

	//
	// data layout
	// +-----------------+ width
	// | Y Y Y Y Y Y Y Y |
	// | Y Y Y Y Y Y Y Y |
	// | Y Y Y Y Y Y Y Y |
	// | Y Y Y Y Y Y Y Y |
	// +---------+-------+ height
	// | U U U U U U U U |
	// | U U U U U U U U |
	// | U U U U U U U U |
	// | U U U U U U U U |
	// +---------+-------+ height
	// | V V V V V V V V |
	// | V V V V V V V V |
	// | V V V V V V V V |
	// | V V V V V V V V |
	// +-----------------+ height
	//

	yBuf := GetByteBuf(ySize)
	uBuf := GetByteBuf(uSize)
	vBuf := GetByteBuf(vSize)

	ret := C.libconvert_to_yuv444(
		(*C.uchar)(unsafe.Pointer(&img.Pix[0])),
		C.int(width),
		C.int(height),
		(*C.uchar)(unsafe.Pointer(&yBuf[0])),
		(*C.uchar)(unsafe.Pointer(&uBuf[0])),
		(*C.uchar)(unsafe.Pointer(&vBuf[0])),
	)
	if int(ret) != 0 {
		PutByteBuf(yBuf)
		PutByteBuf(uBuf)
		PutByteBuf(vBuf)
		return nil, errors.WithStack(ErrConvertToYUV)
	}

	out := &image.YCbCr{
		Y:              yBuf,
		Cb:             uBuf,
		Cr:             vBuf,
		YStride:        width,
		CStride:        width,
		SubsampleRatio: image.YCbCrSubsampleRatio444,
		Rect:           image.Rect(0, 0, width, height),
	}
	return out, nil
}
