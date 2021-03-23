package blurry

import (
	"testing"

	"github.com/anthonynsimon/bild/blur"
	"github.com/disintegration/imaging"
	"github.com/esimov/stackblur-go"
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
		width, height := wh(testImg)

		ch := make([]chan struct{}, tb.N)
		for i := 0; i < len(ch); i += 1 {
			ch[i] = make(chan struct{}, 1)
		}

		tb.ResetTimer()
		for i := 0; i < tb.N; i += 1 {
			_ = stackblur.Process(testImg, uint32(width), uint32(height), 3, ch[i])
		}
	})
	b.Run("blurry/Boxblur", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = Boxblur(testImg, 3)
		}
	})
	b.Run("blurry/Gaussianblur", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = Gaussianblur(testImg, 3.0)
		}
	})
	b.Run("blurry/Boxblur/DisablePool", func(tb *testing.B) {
		DisablePool()
		tb.Cleanup(EnablePool)
		for i := 0; i < tb.N; i += 1 {
			_, _ = Boxblur(testImg, 3)
		}
	})
	b.Run("blurry/Gaussianblur/DisablePool", func(tb *testing.B) {
		DisablePool()
		tb.Cleanup(EnablePool)
		for i := 0; i < tb.N; i += 1 {
			_, _ = Gaussianblur(testImg, 3.0)
		}
	})
}
