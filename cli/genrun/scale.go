package genrun

import (
	"gopkg.in/urfave/cli.v1"
	"strconv"
)

func scaleAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	var cmd string
	switch c.String("filter") {
	case "none":
		cmd = "scale"
	case "box":
		cmd = "scale_box"
	case "linear":
		cmd = "scale_linear"
	case "gaussian":
		cmd = "scale_gaussian"
	}

	args := []string{
		strconv.Itoa(c.Int("scale_width")),
		strconv.Itoa(c.Int("scale_height")),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, args); err != nil {
		return err
	}

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
				Name:  "r,runtime",
				Usage: "halide runtime path",
				Value: "./Halide-Runtime",
			},
			cli.StringFlag{
				Name:  "f,file",
				Usage: "/path/to/blurry.cpp path",
				Value: "./blurry.cpp",
			},
			cli.StringFlag{
				Name:  "filter",
				Usage: "scale filter ('none' or 'box' or 'linear' or 'gaussian')",
				Value: "none",
			},
			cli.IntFlag{
				Name:  "scale_width",
				Usage: "scale width",
				Value: 128,
			},
			cli.IntFlag{
				Name:  "scale_height",
				Usage: "scale height",
				Value: 96,
			},
		},
	})
}
