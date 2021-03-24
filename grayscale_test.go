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
      img, err := Grayscale(testImg)
      if err != nil {
        tb.Fatalf(err.Error())
      }
      PutRGBA(img)
		}
	})
	b.Run("blurry/Grayscale/D", func(tb *testing.B) {
		DisablePool()
		tb.Cleanup(EnablePool)

		tb.ResetTimer()
		for i := 0; i < tb.N; i += 1 {
      img, err := Grayscale(testImg)
      if err != nil {
        tb.Fatalf(err.Error())
      }
      PutRGBA(img)
		}
	})
}
