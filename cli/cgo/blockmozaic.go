package cgo

import (
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func blockmozaicAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

	out, err := blurry.Blockmozaic(in, c.Int("block"))
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
		Name:   "blockmozaic",
		Action: blockmozaicAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.IntFlag{
				Name:  "b,block",
				Usage: "block size",
				Value: 10,
			},
		},
	})
}
