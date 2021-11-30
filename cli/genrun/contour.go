package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func contourAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		c.String("threshold"),
		c.String("size"),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), "contour", args); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "contour",
		Action: contourAction,
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
				Name:  "t, threshold",
				Usage: "threshold for binarizing a sobelized image",
				Value: "100",
			},
			cli.StringFlag{
				Name:  "s, size",
				Usage: "size to be considered contour",
				Value: "4",
			},
		},
	})
}
