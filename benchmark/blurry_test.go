package benchmark

import (
	"bytes"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"io/ioutil"

	_ "embed"
)

//go:embed testdata/src.png
var testImgData []byte

//go:embed testdata/src320x240.png
var blendImgData []byte

var (
	testImg     *image.RGBA
	testImgARGB *image.RGBA
	testImgABGR *image.RGBA
	testImgBGRA *image.RGBA
)

var (
	blendImg     *image.RGBA
	blendImgARGB *image.RGBA
)

func init() {
	if img, err := pngToRGBA(testImgData); err != nil {
		panic(err.Error())
	} else {
		testImg = img
	}

	if img, err := pngToRGBA(blendImgData); err != nil {
		panic(err.Error())
	} else {
		blendImg = img
	}

	if argb, err := RGBAToARGB(testImg); err != nil {
		panic(err.Error())
	} else {
		testImgARGB = argb
	}

	if argb, err := RGBAToARGB(blendImg); err != nil {
		panic(err.Error())
	} else {
		blendImgARGB = argb
	}

	if abgr, err := ARGBToABGR(testImgARGB); err != nil {
		panic(err.Error())
	} else {
		testImgABGR = abgr
	}

	if bgra, err := ARGBToBGRA(testImgARGB); err != nil {
		panic(err.Error())
	} else {
		testImgBGRA = bgra
	}
}

func pngToRGBA(data []byte) (*image.RGBA, error) {
	img, err := png.Decode(bytes.NewReader(data))
	if err != nil {
		return nil, err
	}
	if i, ok := img.(*image.RGBA); ok {
		return i, nil
	}

	b := img.Bounds()
	rgba := image.NewRGBA(b)
	for y := b.Min.Y; y < b.Max.Y; y += 1 {
		for x := b.Min.X; x < b.Max.X; x += 1 {
			c := color.RGBAModel.Convert(img.At(x, y)).(color.RGBA)
			rgba.Set(x, y, c)
		}
	}
	return rgba, nil
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

func saveImageImage(img image.Image) (string, error) {
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

func saveData(img *image.RGBA, prefix string) (string, error) {
	b := img.Bounds()
	w, h := b.Max.X, b.Max.Y

	out, err := ioutil.TempFile("/tmp", fmt.Sprintf("%s_%dx%d_*.raw", prefix, w, h))
	if err != nil {
		return "", err
	}
	defer out.Close()

	if _, err := out.Write(img.Pix); err != nil {
		return "", err
	}
	return out.Name(), nil
}
