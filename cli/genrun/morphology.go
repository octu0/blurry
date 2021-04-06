package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func morphologyAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generate(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	var cmd string
	switch c.String("mode") {
	case "1":
		cmd = "morphology_open"
	case "2":
		cmd = "morphology_close"
	case "3":
		cmd = "morphology_gradient"
	}

	args := []string{
		c.String("size"),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, args); err != nil {
		return err
	}

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
				Usage: "morphology mode(1 = open, 2 = close, 3 = gradient)",
				Value: "1",
			},
			cli.StringFlag{
				Name:  "s,size",
				Usage: "box size",
				Value: "3",
			},
		},
	})
}
