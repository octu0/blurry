package benchmark

import (
	"image"
	"testing"

	"github.com/anthonynsimon/bild/transform"
	"github.com/disintegration/imaging"
	"github.com/octu0/blurry"
)

func BenchmarkCrop(b *testing.B) {
	b.Run("bild/Crop", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Crop(testImg, image.Rect(175, 40, 175+80, 40+50))
		}
	})
	b.Run("imaging/Crop", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Crop(testImg, image.Rect(175, 40, 175+80, 40+50))
		}
	})
	b.Run("libyuv/Crop", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBScaleClip(testImg, 175, 40, 80, 50)
		}
	})
	b.Run("blurry/Crop", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Crop(testImg, image.Pt(175, 40), 80, 50)
		}
	})
}
