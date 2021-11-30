package cgo

import (
	"image"
	"image/color"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func contourAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

	threshold := uint8(c.Int("threshold"))
	size := uint8(c.Int("size"))
	points, err := blurry.Contour(in, threshold, size)
	if err != nil {
		return err
	}
	if c.Bool("render") {
		out := image.NewRGBA(in.Rect)
		copy(out.Pix, in.Pix)

		c := color.RGBA{R: 255, G: 255, B: 255, A: 255}
		for _, pt := range points {
			out.SetRGBA(pt.X, pt.Y, c)
		}
		path, err := saveImage(out)
		if err != nil {
			return err
		}
		log.Printf("info: open %s", path)
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "contour",
		Action: contourAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.IntFlag{
				Name:  "t,threshold",
				Usage: "threshold for binarizing a sobelized image",
				Value: 100,
			},
			cli.IntFlag{
				Name:  "s,size",
				Usage: "size to be considered contour",
				Value: 4,
			},
			cli.BoolFlag{
				Name:  "r,render",
				Usage: "render contour line",
			},
		},
	})
}
