package cgo

import (
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func flipAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

	flipMode := blurry.FlipMode(0)
	switch c.String("flip") {
	case "v", "vertical":
		flipMode = blurry.FlipVertical
	case "h", "horizon":
		flipMode = blurry.FlipHorizon
	}

	out, err := blurry.Flip(in, flipMode)
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
		Name:   "flip",
		Action: flipAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.StringFlag{
				Name:  "flip",
				Usage: "flip direction(v,vertical,h,horizon)",
				Value: "v",
			},
		},
	})
}
