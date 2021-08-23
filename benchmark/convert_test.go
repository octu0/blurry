package benchmark

import (
	"testing"
)

func BenchmarkConvert(b *testing.B) {
	b.Run("libyuv/RGBAtoARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = RGBAToARGB(testImg)
		}
	})
	b.Run("libyuv/ARGBToRGBA", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToRGBA(testImgARGB)
		}
	})
	b.Run("libyuv/ARGBToABGR", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToABGR(testImgARGB)
		}
	})
}
