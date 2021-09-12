package benchmark

import (
	"image"
	"testing"

	"github.com/anthonynsimon/bild/blend"
	"github.com/disintegration/imaging"
	"github.com/octu0/blurry"
)

func BenchmarkBlend(b *testing.B) {
	b.Run("bild/Blend/Normal", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = blend.Normal(testImg, blendImg)
		}
	})
	b.Run("bild/Blend/Add", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = blend.Add(testImg, blendImg)
		}
	})
	b.Run("bild/Blend/Subtract", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = blend.Subtract(testImg, blendImg)
		}
	})
	b.Run("imaging/Paste", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_ = imaging.Paste(testImg, blendImg, image.Pt(0, 0))
		}
	})
	b.Run("libyuv/ARGBAdd", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, err := ARGBAdd(testImgARGB, blendImgARGB)
			if err != nil {
				tb.Fatalf(err.Error())
			}
		}
	})
	b.Run("libyuv/ARGBSubtract", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, err := ARGBSubtract(testImgARGB, blendImgARGB)
			if err != nil {
				tb.Fatalf(err.Error())
			}
		}
	})
	b.Run("blurry/Blend/Normal", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Blend(testImg, blendImg, image.Pt(0, 0), blurry.BlendNormal)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
	b.Run("blurry/Blend/Add", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Blend(testImg, blendImg, image.Pt(0, 0), blurry.BlendAdd)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
	b.Run("blurry/Blend/Sub", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			img, err := blurry.Blend(testImg, blendImg, image.Pt(0, 0), blurry.BlendSub)
			if err != nil {
				tb.Fatalf(err.Error())
			}
			blurry.PutRGBA(img)
		}
	})
}
