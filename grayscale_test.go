package blurry

import (
	"testing"

	"github.com/anthonynsimon/bild/effect"
	"github.com/disintegration/imaging"
)

func BenchmarkGrayscale(b *testing.B) {
	b.Run("bild/Grayscale", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = effect.Grayscale(testImg)
		}
	})
	b.Run("imaging/Grayscale", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Grayscale(testImg)
		}
	})
	b.Run("blurry/Grayscale", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = Grayscale(testImg)
		}
	})
}
