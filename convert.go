package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lconvert_from_abgr_osx -lconvert_from_argb_osx -lconvert_from_bgra_osx -lconvert_from_rabg_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lconvert_from_abgr_linux -lconvert_from_argb_linux -lconvert_from_bgra_linux -lconvert_from_rabg_linux -ldl -lm
#include <stdlib.h>

#ifdef __APPLE__
#include "libconvert_from_argb_osx.h"
#include "libconvert_from_abgr_osx.h"
#include "libconvert_from_bgra_osx.h"
#include "libconvert_from_rabg_osx.h"
#elif __linux__
#include "libconvert_from_argb_linux.h"
#include "libconvert_from_abgr_linux.h"
#include "libconvert_from_bgra_linux.h"
#include "libconvert_from_rabg_linux.h"
#endif

#include "buffer.h"

int call_convert_rgba(
  unsigned char mode,
  halide_buffer_t *in,
  int32_t width, int32_t height,
  halide_buffer_t *out
) {
  if(1 == mode) {
    return convert_from_argb(in, width, height, out);
  }
  if(2 == mode) {
    return convert_from_abgr(in, width, height, out);
  }
  if(3 == mode) {
    return convert_from_bgra(in, width, height, out);
  }
  if(4 == mode) {
    return convert_from_rabg(in, width, height, out);
  }
  return 1;
}

int libconvert_from_rgba(
  unsigned char *src,
  int32_t width, int32_t height,
  unsigned char mode,
  unsigned char *out
) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_rgba_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = call_convert_rgba(mode, in_rgba_buf, width, height, out_rgba_buf);
  free_buf(in_rgba_buf);
  free_buf(out_rgba_buf);
  return ret;
}
*/
import "C"
import (
	"errors"
	"image"
)

type ConvertRGBAMode uint8

const (
	ConvertFromARGB ConvertRGBAMode = iota + 1
	ConvertFromABGR
	ConvertFromBGRA
	ConvertFromRABG
)

var (
	ErrConvertFromRGBA = errors.New("convert_from cgo call error")
)

func ConvertFrom(img *image.RGBA, mode ConvertRGBAMode) (*image.RGBA, error) {
	width, height := wh(img)
	out := GetRGBA(width, height)

	ret := C.libconvert_from_rgba(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.uchar(mode),
		(*C.uchar)(&out.Pix[0]),
	)
	if int(ret) != 0 {
		return nil, ErrConvertFromRGBA
	}
	return out, nil
}

func ConvertFromARGB(img *image.RGBA) (*image.RGBA, error) {
	return ConvertFrom(img, ConvertFromARGB)
}

func ConvertFromABGR(img *image.RGBA) (*image.RGBA, error) {
	return ConvertFrom(img, ConvertFromABGR)
}

func ConvertFromBGRA(img *image.RGBA) (*image.RGBA, error) {
	return ConvertFrom(img, ConvertFromBGRA)
}

func ConvertFromRABG(img *image.RGBA) (*image.RGBA, error) {
	return ConvertFrom(img, ConvertFromRABG)
}
