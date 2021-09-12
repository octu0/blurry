package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/effect"
	"github.com/disintegration/imaging"
	"github.com/octu0/blurry"
)

func BenchmarkEmboss(b *testing.B) {
	b.Run("bild/Emboss", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = effect.Emboss(testImg)
		}
	})
	b.Run("imaging/Convolve3x3", func(tb *testing.B) {
		kernel := [9]float64{
			-1, -1, 0,
			-1, 1, 1,
			0, 1, 1,
		}
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Convolve3x3(testImg, kernel, nil)
		}
	})
	b.Run("blurry/Emboss", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Emboss(testImg)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
}
