package blurry

/*
#cgo CFLAGS: -I${SRCDIR}/include
#cgo darwin LDFLAGS: -L${SRCDIR}/lib -lruntime_osx -lpcm16_decibel_osx -ldl -lm
#cgo linux  LDFLAGS: -L${SRCDIR}/lib -lruntime_linux -lpcm16_decibel_linux -ldl -lm
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include "libpcm16_decibel_osx.h"
#elif __linux__
#include "libpcm16_decibel_linux.h"
#endif

#include "buffer.h"

int libpcm16_decibel(unsigned char *src, int32_t length, unsigned char *out) {
  halide_buffer_t *in_pcm16_buf = create_pcm16_buffer(src, length);
  if(in_pcm16_buf == NULL) {
    return 1;
  }
  halide_buffer_t *out_pcm16_decibel_buf = create_pcm16_decibel_buffer(out, 1);
  if(out_pcm16_decibel_buf == NULL) {
    free_buf(in_pcm16_buf);
    return 1;
  }

  int ret = pcm16_decibel(in_pcm16_buf, length, out_pcm16_decibel_buf);
  free_buf(in_pcm16_buf);
  free_buf(out_pcm16_decibel_buf);
  return ret;
}
*/
import "C"
import (
	"math"
	"unsafe"

	"github.com/octu0/cgobytepool"
	"github.com/pkg/errors"
)

const (
	pcm16Max float64 = 32768.0 // 2^15
)

var (
	ErrPCM16Decibel = errors.New("pcm16_decibel cgo call error")
)

func PCM16DecibelFromInt16(data []int16) (float32, error) {
	b := cgobytepool.ReflectGoBytes(unsafe.Pointer(&data[0]), len(data), cap(data))
	return PCM16Decibel(b, len(data))
}

func PCM16Decibel(data []byte, length int) (float32, error) {
	out := make([]byte, 4) // float
	ret := C.libpcm16_decibel(
		(*C.uchar)(unsafe.Pointer(&data[0])),
		C.int(length),
		(*C.uchar)(unsafe.Pointer(&out[0])),
	)
	if int(ret) != 0 {
		return 0.0, errors.WithStack(ErrPCM16Decibel)
	}
	return *(*float32)(unsafe.Pointer(&out[0])), nil
}

func NativePCM16Decibel(data []int16) float64 {
	// root mean square
	rms := func(pcm []int16) float64 {
		sumValues := float64(0.0)
		for i := 0; i < len(pcm); i += 1 {
			normalize := float64(pcm[i]) / pcm16Max // -1.0 to 1.0
			sumValues += math.Pow(normalize, 2)
		}
		return math.Sqrt(sumValues / float64(len(pcm)))
	}
	return 20 * math.Log10(rms(data))
}
