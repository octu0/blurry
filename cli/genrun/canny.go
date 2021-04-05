package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func cannyAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generate(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		c.String("max"),
		c.String("min"),
		c.String("dilate"),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), "canny", args); err != nil {
		return err
	}

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
				Name:  "max",
				Usage: "threshold max(highly reliable contours)",
				Value: "250",
			},
			cli.StringFlag{
				Name:  "min",
				Usage: "threshold min(maybe noise, to be eliminate)",
				Value: "100",
			},
			cli.StringFlag{
				Name:  "d,dilate",
				Usage: "dilate value(0 is no dilation)",
				Value: "0",
			},
		},
	})
}
