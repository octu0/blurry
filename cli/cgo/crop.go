package cgo

import (
	"image"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func cropAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

	out, err := blurry.Crop(in, image.Pt(c.Int("x"), c.Int("y")), c.Int("cw"), c.Int("ch"))
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
		Name:   "crop",
		Action: cropAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.IntFlag{
				Name:  "x",
				Usage: "crop start pos X",
				Value: 175,
			},
			cli.IntFlag{
				Name:  "y",
				Usage: "crop start pox Y",
				Value: 40,
			},
			cli.IntFlag{
				Name:  "cw,crop_width",
				Usage: "crop width",
				Value: 80,
			},
			cli.IntFlag{
				Name:  "ch,crop_height",
				Usage: "crop_height",
				Value: 50,
			},
		},
	})
}
