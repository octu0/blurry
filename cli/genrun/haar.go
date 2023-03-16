package genrun

import (
	"fmt"

	"gopkg.in/urfave/cli.v1"
)

func haarAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	cmd := fmt.Sprintf("haar_%s", c.String("o"))
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, nil); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "haar",
		Action: haarAction,
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
				Name:  "o,order",
				Usage: "x or y",
				Value: "x",
			},
		},
	})
}
