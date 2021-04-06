package bridge

import (
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func cannyAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

  mode := blurry.CannyMorphologyMode(c.Int("mode"))
  size := c.Int("size")
  dilate := c.Int("dilate")
	out, err := blurry.MorphologyCannyWithDilate(in, c.Int("max"), c.Int("min"), mode, size, dilate)
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
		Name:   "canny",
		Action: cannyAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.IntFlag{
				Name:  "max",
				Usage: "threshold max(highly reliable contours)",
				Value: 250,
			},
			cli.IntFlag{
				Name:  "min",
				Usage: "threshold min(maybe noise, to be eliminate)",
				Value: 100,
			},
			cli.StringFlag{
				Name:  "m,mode",
				Usage: "morphology mode (0=none, 1=open, 2=close)",
				Value: "0",
			},
			cli.IntFlag{
				Name:  "s,size",
				Usage: "morphology size (0 is skip morphology)",
				Value: 0,
			},
			cli.IntFlag{
				Name:  "d,dilate",
				Usage: "dilate value(0 is no dilation)",
				Value: 0,
			},
		},
	})
}
