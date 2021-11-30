package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lconvert_from_yuv_420_osx -lconvert_from_yuv_444_osx -lconvert_to_yuv_420_osx -lconvert_to_yuv_444_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lconvert_from_yuv_420_linux -lconvert_from_yuv_444_linux -lconvert_to_yuv_420_linux -lconvert_to_yuv_444_linux -ldl -lm
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
  unsigned char *out
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_yuv420_buf = create_yuv420_buffer(out, width, height);
  if(out_yuv420_buf == NULL) {
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = convert_to_yuv_420(in_rgba_buf, width, height, out_yuv420_buf);
  free_buf(in_rgba_buf);
  free_buf(out_yuv420_buf);
  return ret;
}

int libconvert_to_yuv444(
  unsigned char *src,
  int32_t width, int32_t height,
  unsigned char *out
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_yuv444_buf = create_yuv444_buffer(out, width, height);
  if(out_yuv444_buf == NULL) {
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = convert_to_yuv_444(in_rgba_buf, width, height, out_yuv444_buf);
  free_buf(in_rgba_buf);
  free_buf(out_yuv444_buf);
  return ret;
}
*/
import "C"
import (
	"errors"
	"image"
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
		(*C.uchar)(&img.Y[0]),
		(*C.uchar)(&img.Cb[0]),
		(*C.uchar)(&img.Cr[0]),
		C.int(img.YStride),
		C.int(img.CStride),
		C.int(img.CStride),
		C.int(width),
		C.int(height),
		C.uchar(chrs),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrConvertFromYUV
	}
	return out, nil
}

func ConvertFromYUV420(img *image.YCbCr) (*image.RGBA, error) {
	if img.SubsampleRatio != image.YCbCrSubsampleRatio420 {
		return nil, ErrYUVSubsampleRateMustI420
	}
	return ConvertFromYUV(img, ChromaSubsampling420)
}

func ConvertFromYUV444(img *image.YCbCr) (*image.RGBA, error) {
	if img.SubsampleRatio != image.YCbCrSubsampleRatio444 {
		return nil, ErrYUVSubsampleRateMustI444
	}
	return ConvertFromYUV(img, ChromaSubsampling444)
}

func ConvertFromYUV420Plane(y, u, v []byte, y_stride, u_stride, v_stride int, width, height int) (*image.RGBA, error) {
	img := &image.YCbCr{
		Y:              y,
		Cb:             u,
		Cr:             v,
		Rect:           image.Rect(0, 0, width, height),
		YStride:        y_stride,
		CStride:        u_stride,
		SubsampleRatio: image.YCbCrSubsampleRatio420,
	}
	return ConvertFromYUV(img, ChromaSubsampling420)
}

func ConvertFromYUV444Plane(y, u, v []byte, y_stride, u_stride, v_stride int, width, height int) (*image.RGBA, error) {
	img := &image.YCbCr{
		Y:              y,
		Cb:             u,
		Cr:             v,
		Rect:           image.Rect(0, 0, width, height),
		YStride:        y_stride,
		CStride:        u_stride,
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
	buf := GetByteBuf((height + uvHeight + uvHeight) * width)
	defer PutByteBuf(buf)

	ret := C.libconvert_to_yuv420(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&buf[0]),
	)
	if int(ret) != 0 {
		return nil, ErrConvertToYUV
	}

	yBuf := GetByteBuf(ySize)
	uBuf := GetByteBuf(uSize)
	vBuf := GetByteBuf(vSize)

	yPlaneSize := width * height
	uPlaneSize := width * uvHeight
	vPlaneSize := width * uvHeight

	yPlane := buf[0:yPlaneSize]
	uPlane := buf[yPlaneSize : yPlaneSize+uPlaneSize]
	vPlane := buf[yPlaneSize+uPlaneSize : yPlaneSize+uPlaneSize+vPlaneSize]

	copy(yBuf, yPlane)

	for i := 0; i < uvHeight; i += 1 {
		s := i * uvWidth
		uRow := uPlane[:width]
		vRow := vPlane[:width]

		copy(uBuf[s:s+uvWidth], uRow[:uvWidth])
		copy(vBuf[s:s+uvWidth], vRow[:uvWidth])

		uPlane = uPlane[width:]
		vPlane = vPlane[width:]
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
	buf := GetByteBuf(ySize + uSize + vSize)
	defer PutByteBuf(buf)

	ret := C.libconvert_to_yuv444(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		(*C.uchar)(&buf[0]),
	)
	if int(ret) != 0 {
		return nil, ErrConvertToYUV
	}

	yBuf := GetByteBuf(ySize)
	uBuf := GetByteBuf(uSize)
	vBuf := GetByteBuf(vSize)

	yPlane := buf[0:ySize]
	uPlane := buf[ySize : ySize+uSize]
	vPlane := buf[ySize+uSize : ySize+uSize+vSize]

	copy(yBuf, yPlane)
	copy(uBuf, uPlane)
	copy(vBuf, vPlane)

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
