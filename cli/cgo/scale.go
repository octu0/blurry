package cgo

import (
	"fmt"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func scaleAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

	var mode blurry.ScaleFilter
	switch c.String("filter") {
	case "none":
		mode = blurry.ScaleFilterNone
	case "box":
		mode = blurry.ScaleFilterBox
	case "linear":
		mode = blurry.ScaleFilterLinear
	case "gaussian":
		mode = blurry.ScaleFilterGaussian
	default:
		return fmt.Errorf("unknown scale mode:%s", c.String("filter"))
	}

	out, err := blurry.Scale(in, c.Int("sw"), c.Int("sh"), mode)
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
		Name:   "scale",
		Action: scaleAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.StringFlag{
				Name:  "filter",
				Usage: "scale filter ('none' or 'box' or 'linear' or 'gaussian')",
				Value: "none",
			},
			cli.IntFlag{
				Name:  "sw,scale_width",
				Usage: "scale width",
				Value: 128,
			},
			cli.IntFlag{
				Name:  "sh,scale_height",
				Usage: "scale height",
				Value: 96,
			},
		},
	})
}
