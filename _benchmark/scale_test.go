package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/transform"
	"github.com/disintegration/imaging"
	"github.com/octu0/blurry"
)

func BenchmarkScale(b *testing.B) {
	b.Run("bild/Resize/NearestNeighbor", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Resize(testImg, 128, 96, transform.NearestNeighbor)
		}
	})
	b.Run("bild/Resize/Box", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Resize(testImg, 128, 96, transform.Box)
		}
	})
	b.Run("bild/Resize/Linear", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Resize(testImg, 128, 96, transform.Linear)
		}
	})
	b.Run("bild/Resize/Gaussian", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = transform.Resize(testImg, 128, 96, transform.Gaussian)
		}
	})
	b.Run("imaging/Resize/NearestNeighbor", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Resize(testImg, 128, 96, imaging.NearestNeighbor)
		}
	})
	b.Run("imaging/Resize/Box", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Resize(testImg, 128, 96, imaging.Box)
		}
	})
	b.Run("imaging/Resize/Linear", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Resize(testImg, 128, 96, imaging.Linear)
		}
	})
	b.Run("imaging/Resize/Gaussian", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Resize(testImg, 128, 96, imaging.Gaussian)
		}
	})
	b.Run("libyuv/Scale/None", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBScale(testImg, 128, 96, FilterNone)
		}
	})
	b.Run("libyuv/Scale/Box", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBScale(testImg, 128, 96, FilterBox)
		}
	})
	b.Run("libyuv/Scale/Linear", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBScale(testImg, 128, 96, FilterLinear)
		}
	})
	b.Run("blurry/Scale/None", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Scale(testImg, 128, 96, blurry.ScaleFilterNone)
		}
	})
	b.Run("blurry/Scale/Box", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Scale(testImg, 128, 96, blurry.ScaleFilterBox)
		}
	})
	b.Run("blurry/Scale/Linear", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Scale(testImg, 128, 96, blurry.ScaleFilterLinear)
		}
	})
	b.Run("blurry/Scale/Gaussian", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.Scale(testImg, 128, 96, blurry.ScaleFilterGaussian)
		}
	})
}
