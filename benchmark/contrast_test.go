package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/adjust"
	"github.com/disintegration/imaging"
  "github.com/octu0/blurry"
)

func BenchmarkContrast(b *testing.B) {
	b.Run("bild/Contrast", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = adjust.Contrast(testImg, -0.5)
		}
	})
	b.Run("imaging/Contrast", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.AdjustContrast(testImg, 20)
		}
	})
	b.Run("blurry/Contrast", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
      img, err := blurry.Contrast(testImg, 0.525)
      if err != nil {
        tb.Fatalf(err.Error())
      }
      blurry.PutRGBA(img)
		}
	})
	b.Run("blurry/Contrast/D", func(tb *testing.B) {
		blurry.DisablePool()
		tb.Cleanup(blurry.EnablePool)

		tb.ResetTimer()
		for i := 0; i < tb.N; i += 1 {
      img, err := blurry.Contrast(testImg, 0.525)
      if err != nil {
        tb.Fatalf(err.Error())
      }
      blurry.PutRGBA(img)
		}
	})
}
