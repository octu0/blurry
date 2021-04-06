package bridge

import (
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func morphologyAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

	mode := blurry.MorphologyMode(c.Int("mode"))
	out, err := blurry.Morphology(in, mode, c.Int("size"), c.Int("count"))
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
		Name:   "morphology",
		Action: morphologyAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.IntFlag{
				Name:  "m,mode",
				Usage: "morphology mode(1=open,2=close,3=gradient)",
				Value: 1,
			},
			cli.IntFlag{
				Name:  "s,size",
				Usage: "box size",
				Value: 3,
			},
			cli.IntFlag{
				Name:  "c,count",
				Usage: "iteration count",
				Value: 5,
			},
		},
	})
}
