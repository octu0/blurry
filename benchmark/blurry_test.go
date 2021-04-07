package benchmark

import (
	"bytes"
	"image"
	"image/color"
	"image/png"
	"io/ioutil"

	_ "embed"
)

//go:embed testdata/src.png
var testImgData []byte

var testImg *image.RGBA
var testImgARGB *image.RGBA

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

	argb, err := RGBAToARGB(testImg)
	if err != nil {
		panic("testImg RGBA to ARGB error:" + err.Error())
	}
	testImgARGB = argb
}

func saveImage(img *image.RGBA) (string, error) {
	out, err := ioutil.TempFile("/tmp", "out*.png")
	if err != nil {
		return "", err
	}
	defer out.Close()

	if err := png.Encode(out, img); err != nil {
		return "", err
	}
	return out.Name(), nil
}

func saveImageFromARGB(img *image.RGBA) (string, error) {
	rgba, err := ARGBToRGBA(img)
	if err != nil {
		return "", err
	}
	return saveImage(rgba)
}
