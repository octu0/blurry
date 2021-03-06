package benchmark

import (
	"testing"

	"github.com/anthonynsimon/bild/blur"
	"github.com/disintegration/imaging"
	"github.com/esimov/stackblur-go"
	"github.com/octu0/blurry"
)

func BenchmarkBlur(b *testing.B) {
	b.Run("bild/blur/Box", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = blur.Box(testImg, 3.0)
		}
	})
	b.Run("bild/blur/Gaussian", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = blur.Gaussian(testImg, 3.0)
		}
	})
	b.Run("imaging/Blur", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Blur(testImg, 1.5)
		}
	})
	b.Run("stackblur-go", func(tb *testing.B) {
		b := testImg.Bounds()
		width, height := b.Max.X, b.Max.Y
		ch := make([]chan struct{}, tb.N)
		for i := 0; i < len(ch); i += 1 {
			ch[i] = make(chan struct{}, 1)
		}

		tb.ResetTimer()
		for i := 0; i < tb.N; i += 1 {
			_ = stackblur.Process(testImg, uint32(width), uint32(height), 3, ch[i])
		}
	})
	b.Run("libyuv/ARGBBlur", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBBlur(testImgARGB, 5)
		}
	})
	b.Run("blurry/Boxblur", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Boxblur(testImg, 3)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
	b.Run("blurry/Gaussianblur", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Gaussianblur(testImg, 3.0)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
	b.Run("blurry/Boxblur/D", func(tb *testing.B) {
		blurry.DisablePool()
		tb.Cleanup(blurry.EnablePool)
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Boxblur(testImg, 3)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
	b.Run("blurry/Gaussianblur/D", func(tb *testing.B) {
		blurry.DisablePool()
		tb.Cleanup(blurry.EnablePool)
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Gaussianblur(testImg, 3.0)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
}
