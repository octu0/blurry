package blurry

/*
#cgo CFLAGS: -I.
#cgo darwin LDFLAGS: -L. -lruntime_osx -lclone_osx -ldl -lm
#cgo linux  LDFLAGS: -L. -lruntime_linux -lclone_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#ifdef __APPLE__
#include "libclone_osx.h"
#elif __linux__
#include "libclone_linux.h"
#endif

int libclone(unsigned char *src, int32_t width, int32_t height, unsigned char *out) {
  halide_buffer_t in_rgba_buf = {0};
  halide_buffer_t out_rgba_buf = {0};

  bind_rgba_buf(&in_rgba_buf, src, width, height);
  bind_rgba_buf(&out_rgba_buf, out, width, height);

  int ret = clone(&in_rgba_buf, width, height, &out_rgba_buf);
  free_buf(&in_rgba_buf);
  free_buf(&out_rgba_buf);
  return ret;
}
*/
import "C"
import(
  "errors"
  "image"
)

var(
  ErrClone = errors.New("clone cgo call error")
)

func Clone(img *image.RGBA) (*image.RGBA, error) {
  width, height := wh(img)
  out := GetNRGBA(width, height)

  ret := C.libclone(
    (*C.uchar)(&img.Pix[0]),
    C.int(width),
    C.int(height),
    (*C.uchar)(&out.Pix[0]),
  )
  if int(ret) != 0 {
    return nil, ErrClone
  }
  return out, nil
}
