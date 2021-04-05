package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func dilationAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generate(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		c.String("size"),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), "dilation", args); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "dilation",
		Action: dilationAction,
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
				Name:  "s, size",
				Usage: "box size",
				Value: "8",
			},
		},
	})
}
