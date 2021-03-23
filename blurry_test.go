package blurry

import (
	"bytes"
	"image"
	"image/color"
	"image/png"

	_ "embed"
)

//go:embed testdata/src.png
var testImgData []byte

var testImg *image.RGBA

func init() {
	img, err := png.Decode(bytes.NewReader(testImgData))
	if err != nil {
		panic("test img data load error:" + err.Error())
	}
	if i, ok := img.(*image.RGBA); ok {
		testImg = i
		return
	}

	b := img.Bounds()
	testImg = image.NewRGBA(b)
	for y := b.Min.Y; y < b.Max.Y; y += 1 {
		for x := b.Min.X; x < b.Max.X; x += 1 {
			c := color.RGBAModel.Convert(img.At(x, y)).(color.RGBA)
			testImg.Set(x, y, c)
		}
	}
}
