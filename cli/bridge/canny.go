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

	out, err := blurry.Canny(in, c.Int("max"), c.Int("min"), c.Float64("sigma"))
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
			cli.Float64Flag{
				Name:  "s,sigma",
				Usage: "sigma",
				Value: 5.0,
			},
		},
	})
}
