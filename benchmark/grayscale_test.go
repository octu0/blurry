package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/effect"
	"github.com/disintegration/imaging"
	"github.com/octu0/blurry"
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
	b.Run("libyuv/ARGBGrayTo", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBGrayTo(testImgARGB)
		}
	})
	b.Run("blurry/Grayscale", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Grayscale(testImg)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
	b.Run("blurry/Grayscale/D", func(tb *testing.B) {
		blurry.DisablePool()
		tb.Cleanup(blurry.EnablePool)

		tb.ResetTimer()
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Grayscale(testImg)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
}
