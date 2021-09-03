package cgo

import (
	"fmt"
	"image"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func blendAction(c *cli.Context) error {
	in0, err := loadImage(c.String("input0"))
	if err != nil {
		return err
	}
	in1, err := loadImage(c.String("input1"))
	if err != nil {
		return err
	}

	var mode blurry.BlendMode
	switch c.String("mode") {
	case "normal":
		mode = blurry.BlendNormal
	case "sub":
		mode = blurry.BlendSub
	case "add":
		mode = blurry.BlendAdd
	case "diff":
		mode = blurry.BlendDiff
	default:
		return fmt.Errorf("unknown blend mode:%s", c.String("mode"))
	}

	pt := image.Pt(c.Int("x"), c.Int("y"))

	out, err := blurry.Blend(in0, in1, pt, mode)
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
		Name:   "blend",
		Action: blendAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i0,input0",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.StringFlag{
				Name:  "i1,input1",
				Usage: "/path/to/input image",
				Value: "./testdata/src160x160.png",
			},
			cli.StringFlag{
				Name:  "m,mode",
				Usage: "blend mode ('normal' or 'add' or 'sub' or 'diff')",
				Value: "normal",
			},
			cli.IntFlag{
				Name:  "x,point-x",
				Usage: "destination point X",
				Value: 0,
			},
			cli.IntFlag{
				Name:  "y,point-y",
				Usage: "destination point Y",
				Value: 0,
			},
		},
	})
}
