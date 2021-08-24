package benchmark

import (
	"testing"
)

func BenchmarkConvert(b *testing.B) {
	b.Run("libyuv/RGBAtoARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = RGBAToARGB(testImg)
		}
	})
	b.Run("libyuv/ARGBToRGBA", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToRGBA(testImgARGB)
		}
	})
	b.Run("libyuv/ARGBToABGR", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToABGR(testImgARGB)
		}
	})
	b.Run("libyuv/ARGBToBGRA", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToBGRA(testImgARGB)
		}
	})
}

func TestConvertLog(*testing.T) {
	// little endian
	println("RGBA", testImg.Pix[0], testImg.Pix[1], testImg.Pix[2], testImg.Pix[3])
	println("ARGB", testImgARGB.Pix[0], testImgARGB.Pix[1], testImgARGB.Pix[2], testImgARGB.Pix[3])
	println("ABGR", testImgABGR.Pix[0], testImgABGR.Pix[1], testImgABGR.Pix[2], testImgABGR.Pix[3])
	println("BGRA", testImgBGRA.Pix[0], testImgBGRA.Pix[1], testImgBGRA.Pix[2], testImgBGRA.Pix[3])
}
