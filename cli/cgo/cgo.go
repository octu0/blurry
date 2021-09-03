package cgo

import (
	"bytes"
	"fmt"
	"image"
	"image/color"
	"image/jpeg"
	"image/png"
	"io"
	"io/ioutil"
	"os"
	"path/filepath"

	"gopkg.in/urfave/cli.v1"
)

var commands []cli.Command

func addCommand(cmd cli.Command) {
	commands = append(commands, cmd)
}

func Command() []cli.Command {
	return commands
}

func loadImage(inputFile string) (*image.RGBA, error) {
	f, err := os.Open(inputFile)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	content, err := io.ReadAll(f)
	if err != nil {
		return nil, err
	}

	img, err := decode(content, filepath.Ext(inputFile))
	if err != nil {
		return nil, err
	}
	return convertRGBA(img), nil
}

func loadData(path string) ([]byte, error) {
	return os.ReadFile(path)
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

func decode(data []byte, fileExt string) (image.Image, error) {
	switch fileExt {
	case ".png":
		d, err := png.Decode(bytes.NewReader(data))
		if err != nil {
			return nil, err
		}
		return d, nil
	case ".jpg":
		d, err := jpeg.Decode(bytes.NewReader(data))
		if err != nil {
			return nil, err
		}
		return d, nil
	default:
		return nil, fmt.Errorf("not support type: %s", fileExt)
	}
}

func convertRGBA(img image.Image) *image.RGBA {
	if i, ok := img.(*image.RGBA); ok {
		return i
	}

	b := img.Bounds()
	out := image.NewRGBA(b)
	for y := b.Min.Y; y < b.Max.Y; y += 1 {
		for x := b.Min.X; x < b.Max.X; x += 1 {
			c := color.RGBAModel.Convert(img.At(x, y)).(color.RGBA)
			out.Set(x, y, c)
		}
	}
	return out
}

func renderBox(dst *image.RGBA, startX, endX int, startY, endY int, c color.RGBA) {
	// draw row : start
	for x := startX; x < endX; x += 1 {
		dst.SetRGBA(x, startY, c)
	}
	// draw row : end
	for x := startX; x < endX; x += 1 {
		dst.SetRGBA(x, endY, c)
	}
	// draw col : start
	for y := startY; y < endY; y += 1 {
		dst.SetRGBA(startX, y, c)
	}
	// draw col : end
	for y := startY; y < endY; y += 1 {
		dst.SetRGBA(endX, y, c)
	}
}
