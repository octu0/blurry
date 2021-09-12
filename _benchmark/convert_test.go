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
	b.Run("libyuv/RGBAToI420", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			// ABGR
			// | <le>
			// RGBA
			// |
			// I420
			_, _ = RGBAToI420(testImgABGR)
		}
	})
	b.Run("libyuv/ABGRToI420", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			// RGBA
			// | <le>
			// ABGR
			// |
			// I420
			_, _ = ABGRToI420(testImgRGBA)
		}
	})
}

func TestConvertLog(t *testing.T) {
	// little endian
	t.Logf("RAW %d %d %d %d", testImg.Pix[0], testImg.Pix[1], testImg.Pix[2], testImg.Pix[3])
	t.Logf("RGBA %d %d %d %d", testImgRGBA.Pix[0], testImgRGBA.Pix[1], testImgRGBA.Pix[2], testImgRGBA.Pix[3])
	t.Logf("ARGB %d %d %d %d", testImgARGB.Pix[0], testImgARGB.Pix[1], testImgARGB.Pix[2], testImgARGB.Pix[3])
	t.Logf("ABGR %d %d %d %d", testImgABGR.Pix[0], testImgABGR.Pix[1], testImgABGR.Pix[2], testImgABGR.Pix[3])
	t.Logf("BGRA %d %d %d %d", testImgBGRA.Pix[0], testImgBGRA.Pix[1], testImgBGRA.Pix[2], testImgBGRA.Pix[3])
}
