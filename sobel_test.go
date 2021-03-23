package blurry

import (
	"testing"

	"github.com/anthonynsimon/bild/effect"
)

func BenchmarkSobel(b *testing.B) {
	b.Run("bild/Sobel", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = effect.Sobel(testImg)
		}
	})
	b.Run("blurry/Sobel", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = Sobel(testImg)
		}
	})
}
