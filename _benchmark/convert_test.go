package benchmark

import (
	"github.com/octu0/blurry"
	"image"
	"testing"
)

func BenchmarkConvert(b *testing.B) {
	var (
		testImgRGBA *image.RGBA
		testImgABGR *image.RGBA
		testImgBGRA *image.RGBA
		testImgI420 *image.YCbCr
		testImgI444 *image.YCbCr
	)
	if rgba, err := ARGBToRGBA(testImgARGB); err != nil {
		b.Errorf(err.Error())
	} else {
		testImgRGBA = rgba
	}
	if abgr, err := ARGBToABGR(testImgARGB); err != nil {
		b.Errorf(err.Error())
	} else {
		testImgABGR = abgr
	}
	if bgra, err := ARGBToBGRA(testImgARGB); err != nil {
		b.Errorf(err.Error())
	} else {
		testImgBGRA = bgra
	}

	if ycbcr, err := RGBAToYCbCrImage420(testImg); err != nil {
		b.Errorf(err.Error())
	} else {
		testImgI420 = ycbcr
	}
	if ycbcr, err := RGBAToYCbCrImage444(testImg); err != nil {
		b.Errorf(err.Error())
	} else {
		testImgI444 = ycbcr
	}
	b.ResetTimer()

	b.Run("libyuv/RGBAtoARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = RGBAToARGB(testImgRGBA)
		}
	})
	b.Run("libyuv/ARGBToRGBA", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToRGBA(testImgARGB)
		}
	})
	b.Run("libyuv/ABGRToARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ABGRToARGB(testImgABGR)
		}
	})
	b.Run("libyuv/BGRAToARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = BGRAToARGB(testImgBGRA)
		}
	})
	b.Run("libyuv/I420ToARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = I420ToARGB(testImgI420)
		}
	})
	b.Run("libyuv/I444ToARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = I444ToARGB(testImgI420)
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
	b.Run("libyuv/ARGBToI420", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToI420(testImgARGB)
		}
	})
	b.Run("libyuv/ARGBToI444", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = ARGBToI444(testImgARGB)
		}
	})
	b.Run("blurry/ConvertFromARGB", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.ConvertFromARGB(testImgARGB)
		}
	})
	b.Run("blurry/ConvertFromABGR", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.ConvertFromABGR(testImgABGR)
		}
	})
	b.Run("blurry/ConvertFromBGRA", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.ConvertFromBGRA(testImgBGRA)
		}
	})
	b.Run("blurry/ConvertFromYUV420", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.ConvertFromYUV420(testImgI420)
		}
	})
	b.Run("blurry/ConvertFromYUV444", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.ConvertFromYUV444(testImgI444)
		}
	})
	b.Run("blurry/ConvertToYUV444", func(tb *testing.B) {
		for i := 0; i < tb.N; i += 1 {
			_, _ = blurry.ConvertToYUV444(testImg)
		}
	})
}

func TestConvertLog(t *testing.T) {
	var (
		testImgRGBA *image.RGBA
		testImgABGR *image.RGBA
		testImgBGRA *image.RGBA
	)
	if rgba, err := ARGBToRGBA(testImgARGB); err != nil {
		t.Errorf(err.Error())
	} else {
		testImgRGBA = rgba
	}
	if abgr, err := ARGBToABGR(testImgARGB); err != nil {
		t.Errorf(err.Error())
	} else {
		testImgABGR = abgr
	}
	if bgra, err := ARGBToBGRA(testImgARGB); err != nil {
		t.Errorf(err.Error())
	} else {
		testImgBGRA = bgra
	}
	// little endian
	t.Logf("RAW %d %d %d %d", testImg.Pix[0], testImg.Pix[1], testImg.Pix[2], testImg.Pix[3])
	t.Logf("RGBA %d %d %d %d", testImgRGBA.Pix[0], testImgRGBA.Pix[1], testImgRGBA.Pix[2], testImgRGBA.Pix[3])
	t.Logf("ARGB %d %d %d %d", testImgARGB.Pix[0], testImgARGB.Pix[1], testImgARGB.Pix[2], testImgARGB.Pix[3])
	t.Logf("ABGR %d %d %d %d", testImgABGR.Pix[0], testImgABGR.Pix[1], testImgABGR.Pix[2], testImgABGR.Pix[3])
	t.Logf("BGRA %d %d %d %d", testImgBGRA.Pix[0], testImgBGRA.Pix[1], testImgBGRA.Pix[2], testImgBGRA.Pix[3])
}
