package cgo

import (
	"fmt"
	"image"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func convert(colorModel string, data []byte, width int, height int) (*image.RGBA, error) {
	in := &image.RGBA{
		Pix:    data,
		Rect:   image.Rect(0, 0, width, height),
		Stride: width,
	}

	switch colorModel {
	case "argb":
		return blurry.ConvertFromARGB(in)
	case "abgr":
		return blurry.ConvertFromABGR(in)
	case "bgra":
		return blurry.ConvertFromBGRA(in)
	case "rabg":
		return blurry.ConvertFromRABG(in)
	}
	return nil, fmt.Errorf("unknown color model: '%s'", colorModel)
}

func convertAction(c *cli.Context) error {
	data, err := loadData(c.String("input"))
	if err != nil {
		return err
	}

	out, err := convert(c.String("model"), data, c.Int("width"), c.Int("height"))
	if err != nil {
		return err
	}

	path, err := saveImage(out)
	if err != nil {
		return err
	}
	log.Printf("info: open %s", path)
	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "convert",
		Action: convertAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/argb_320x240.raw",
			},
			cli.IntFlag{
				Name:  "width",
				Usage: "image width",
				Value: 320,
			},
			cli.IntFlag{
				Name:  "height",
				Usage: "image height",
				Value: 240,
			},
			cli.StringFlag{
				Name:  "m,model",
				Usage: "from color model(argb, abgr, bgra, ragb)",
				Value: "argb",
			},
		},
	})
}
