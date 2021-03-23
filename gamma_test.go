package blurry

import (
	"testing"

	"github.com/anthonynsimon/bild/adjust"
	"github.com/disintegration/imaging"
)

func BenchmarkGamma(b *testing.B) {
	b.Run("bild/Gamma", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = adjust.Gamma(testImg, 2.5)
		}
	})
	b.Run("imaging/Gamma", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.AdjustGamma(testImg, 2.5)
		}
	})
	b.Run("blurry/Gamma", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = Gamma(testImg, 2.5)
		}
	})
}
