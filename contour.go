package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lcontour_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lcontour_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libcontour_osx.h"
#elif __linux__
#include "libcontour_linux.h"
#endif

#include "buffer.h"

int libcontour(unsigned char *src, int32_t width, int32_t height, uint8_t threshold, uint8_t size, unsigned char *out) {
  halide_buffer_t *in_rgba_buf = create_rgba_buffer(src, width, height);
  if(in_rgba_buf == NULL){
    return 1;
  }
  halide_buffer_t *out_rgba_buf = create_uint8_array_buffer(out, width, height);
  if(out_rgba_buf == NULL){
    free_buf(in_rgba_buf);
    return 1;
  }

  int ret = contour(in_rgba_buf, width, height, threshold, size, out_rgba_buf);
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

var (
	ErrContour = errors.New("contour cgo call error")
)

func Contour(img *image.RGBA, threshold uint8, size uint8) ([]image.Point, error) {
	width, height := wh(img)
	buf := GetByteBuf(width * height)
	defer PutByteBuf(buf)

	ret := C.libcontour(
		(*C.uchar)(&img.Pix[0]),
		C.int(width),
		C.int(height),
		C.uchar(threshold),
		C.uchar(size),
		(*C.uchar)(&buf[0]),
	)
	if int(ret) != 0 {
		return nil, ErrContour
	}

	points := make([]image.Point, 0, width*height)
	for y := 0; y < height; y += 1 {
		row := buf[:width]
		for x := 0; x < width; x += 1 {
			if row[x] == 1 {
				points = append(points, image.Pt(x, y))
			}
		}
		buf = buf[width:]
	}
	return points, nil
}
