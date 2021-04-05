package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/effect"
  "github.com/octu0/blurry"
)

func BenchmarkSobel(b *testing.B) {
	b.Run("bild/Sobel", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = effect.Sobel(testImg)
		}
	})
  b.Run("libyuv/ARGBSobel", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
      _, _ = ARGBSobel(testImg)
    }
  })
	b.Run("blurry/Sobel", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Sobel(testImg)
		}
	})
}
