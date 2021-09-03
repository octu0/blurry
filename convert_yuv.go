package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lconvert_from_yuv_i420_osx -lconvert_from_yuv_i444_osx -lconvert_to_yuv_i444_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lconvert_from_yuv_i420_linux -lconvert_from_yuv_i444_linux -lconvert_to_yuv_i444_linux -ldl -lm
#include <stdlib.h>

#ifdef __APPLE__
#include "libconvert_from_yuv_i420_osx.h"
#include "libconvert_from_yuv_i444_osx.h"
#include "libconvert_to_yuv_i444_osx.h"
#elif __linux__
#include "libconvert_from_yuv_i420_linux.h"
#include "libconvert_from_yuv_i444_linux.h"
#include "libconvert_to_yuv_i444_linux.h"
#endif

#include "buffer.h"

int call_convert_from_yuv(
  unsigned char mode,
  halide_buffer_t *in_y_buf,
  halide_buffer_t *in_u_buf,
  halide_buffer_t *in_v_buf,
  int32_t width, int32_t height,
  halide_buffer_t *out_buf
) {
  if(1 == mode) {
    return convert_from_yuv_i420(in_y_buf, in_u_buf, in_v_buf, width, height, out_buf);
  }
  if(2 == mode) {
    return convert_from_yuv_i444(in_y_buf, in_u_buf, in_v_buf, width, height, out_buf);
  }
  return 1;
}

halide_buffer_t *create_yuv_y_plane(
  unsigned char mode, unsigned char *src, int32_t stride,
  int32_t width, int32_t height
) {
  return create_yuv_plane_buffer(src, stride, width, height);
}

halide_buffer_t *create_yuv_uv_plane(
  unsigned char mode, unsigned char *src, int32_t stride,
  int32_t width, int32_t height
) {
  if(1 == mode) {
    return create_yuv_plane_buffer(src, stride, width / 2, height / 2);
  }
  if(2 == mode) {
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
  unsigned char mode,
  unsigned char *out
) {
  halide_buffer_t *in_y_buf = create_yuv_y_plane(mode, src_y, y_stride, width, height);
  if(in_y_buf == NULL){
    return 1;
  }

  halide_buffer_t *in_u_buf = create_yuv_uv_plane(mode, src_u, u_stride, width, height);
  if(in_u_buf == NULL){
    free_buf(in_y_buf);
    return 1;
  }

  halide_buffer_t *in_v_buf = create_yuv_uv_plane(mode, src_v, v_stride, width, height);
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

  int ret = call_convert_from_yuv(mode, in_y_buf, in_u_buf, in_v_buf, width, height, out_rgba_buf);
  free_buf(in_y_buf);
  free_buf(in_u_buf);
  free_buf(in_v_buf);
  free_buf(out_rgba_buf);
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

  int ret = convert_to_yuv_i444(src, width, height, out);
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

type ConvertYUVMode uint8

const(
  ConvertFromI420 ConvertYUVMode = iota + 1
  ConvertFromI444
)

var(
  ErrConvertFromYUV = errors.New("convert_from cgo call error")
  ErrConvertToYUV = errors.New("convert_to cgo call error")
  ErrYUVSubsampleRateMustI420 = errors.New("image.YCbCr.SubsampleRatio must be 420")
  ErrYUVSubsampleRateMustI444 = errors.New("image.YCbCr.SubsampleRatio must be 444")
)

func ConvertFromYUV(img *image.YCbCr, mode ConvertYUVMode) (*image.RGBA, error) {
	width, height := whYCbCr(img)
	out := GetRGBA(width, height)

  ret := C.libconvert_from_yuv(
    (*C.uchar)(&img.Y[0]),
    (*C.uchar)(&img.Cb[0]),
    (*C.uchar)(&img.Cr[0]),
    C.int(img.YStride),
    C.int(img.CStride),
    C.int(width),
    C.int(height),
    (*C.uchar)(&out.Pix[0]),
  )
  if int(ret) != 0 {
    return nil, ErrConvertFromYUV
  }
  return out, nil
}

func ConvertFromI420(img *image.YCbCr) (*image.RGBA, error) {
  if img.SubsampleRatio != image.YCbCrSubsampleRatio420 {
    return nil, ErrYUVSubsampleRateMustI420
  }
  return ConvertFromYUV(img, ConvertFromI420)
}

func ConvertFromI444(img *image.YCbCr) (*image.RGBA, error) {
  if img.SubsampleRatio != image.YCbCrSubsampleRatio444 {
    return nil, ErrYUVSubsampleRateMustI444
  }
  return ConvertFromYUV(img, ConvertFromI444)
}

func ConvertFromI420Plane(y []byte, u []byte, v []byte, y_stride int, u stride int, v stride) (*image.RGBA, error) {
  img := &image.YCbCr{
    Y:       y,
    Cb:      u,
    Cr:      v,
    Rect:    image.Rect(0, 0, width, height),
    YStride: y_stride,
    CStride: u_stride,
    SubsampleRatio: image.YCbCrSubsampleRatio420,
  }
  return ConvertFromYUV(img, ConvertFromI420)
}

func ConvertFromI444Plane(y []byte, u []byte, v []byte, y_stride int, u stride int, v stride) (*image.RGBA, error) {
  img := &image.YCbCr{
    Y:       y,
    Cb:      u,
    Cr:      v,
    Rect:    image.Rect(0, 0, width, height),
    YStride: y_stride,
    CStride: u_stride,
    SubsampleRatio: image.YCbCrSubsampleRatio444,
  }
  return ConvertFromYUV(img, ConvertFromI444)
}

func ConvertToI444(img *image.RGBA) (*image.YCbCr, error) {
	width, height := wh(img)
  ySize := width * height
  uSize := width * height
  vSize := width * height

  buf := GetByteBuf(ySize + uSize + vSize)

  ret := C.libconvert_to_yuv444(
    (*C.uchar)(&img.Pix[0]),
    C.int(width),
    C.int(height),
    (*C.uchar)(&buf[0]),
  )
  if int(ret) != 0 {
    return nil, ErrConvertToYUV
  }

  out := &image.YCbCr{
    Y:       buf[0            : ySize],
    Cb:      buf[ySize        : ySize + uSize],
    Cr:      buf[ySize + uSize: ySize + uSize + vSize],
    YStride: width,
    CStride: width,
    SubsampleRatio: image.YCbCrSubsampleRatio444,
  }
  return out, nil
}
