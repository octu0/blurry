package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func rotateAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	cmd := "rotate" + c.String("rotate")
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, nil); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "rotate",
		Action: rotateAction,
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
				Name:  "rotate",
				Usage: "rotation degrees(0 or 90 or 180 or 270)",
				Value: "90",
			},
		},
	})
}
