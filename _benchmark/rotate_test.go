package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/transform"
	"github.com/disintegration/imaging"
	"github.com/octu0/blurry"
)

func BenchmarkRotate(b *testing.B) {
	b.Run("bild/Rotate/90", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Rotate(testImg, 90, nil)
		}
	})
	b.Run("bild/Rotate/180", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Rotate(testImg, 180, nil)
		}
	})
	b.Run("bild/Rotate/270", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Rotate(testImg, 270, nil)
		}
	})
	b.Run("imaging/90", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Rotate90(testImg)
		}
	})
	b.Run("imaging/180", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Rotate180(testImg)
		}
	})
	b.Run("imaging/270", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Rotate270(testImg)
		}
	})
	b.Run("libyuv/ARGBRotate/90", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBRotate(testImgARGB, Rotate90)
		}
	})
	b.Run("libyuv/ARGBRotate/180", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBRotate(testImgARGB, Rotate180)
		}
	})
	b.Run("libyuv/ARGBRotate/270", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBRotate(testImgARGB, Rotate270)
		}
	})

	b.Run("blurry/Rotate/90", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Rotate(testImg, blurry.Rotate90)
		}
	})
	b.Run("blurry/Rotate/180", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Rotate(testImg, blurry.Rotate180)
		}
	})
	b.Run("blurry/Rotate/270", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Rotate(testImg, blurry.Rotate270)
		}
	})
}
