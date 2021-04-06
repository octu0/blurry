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

	out, err := blurry.CannyWithDilate(in, c.Int("max"), c.Int("min"), c.Int("dilate"))
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
			cli.IntFlag{
				Name:  "d,dilate",
				Usage: "dilate value(0 is no dilation)",
				Value: 0,
			},
		},
	})
}