package blurry

import (
	"testing"

	"github.com/anthonynsimon/bild/adjust"
	"github.com/disintegration/imaging"
)

func BenchmarkContrast(b *testing.B) {
	b.Run("bild/Contrast", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = adjust.Contrast(testImg, -0.5)
		}
	})
	b.Run("imaging/AdjustContrast", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.AdjustContrast(testImg, 20)
		}
	})
	b.Run("blurry/Contrast", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = Contrast(testImg, 0.525)
		}
	})
	b.Run("blurry/Contrast/DisablePool", func(tb *testing.B) {
		DisablePool()
		tb.Cleanup(EnablePool)

		tb.ResetTimer()
		for i := 0; i < tb.N; i += 1 {
			_, _ = Contrast(testImg, 0.525)
		}
	})
}
