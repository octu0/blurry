package genrun

import (
	"gopkg.in/urfave/cli.v1"
	"strconv"
)

func scaleAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generate(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	var cmd string
	switch c.String("mode") {
	case "1":
		cmd = "scale"
	case "2":
		cmd = "scale_box"
	case "3":
		cmd = "scale_linear"
	case "4":
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
				Name:  "m,mode",
				Usage: "scale mode(1 = none, 2 = box, 3 = linear, 4 = gaussian)",
				Value: "1",
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
