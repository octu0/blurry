package genrun

import (
	"gopkg.in/urfave/cli.v1"
	"strconv"
)

func cropAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generate(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		strconv.Itoa(c.Int("x")),
		strconv.Itoa(c.Int("y")),
		strconv.Itoa(c.Int("crop_width")),
		strconv.Itoa(c.Int("crop_height")),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), "crop", args); err != nil {
		return err
	}

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
			cli.StringFlag{
				Name:  "r,runtime",
				Usage: "halide runtime path",
				Value: "./Halide-Runtime",
			},
			cli.StringFlag{
				Name:  "f,file",
				Usage: "/path/to/blurry.cpp path",
				Value: "./blurry.cpp",
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
