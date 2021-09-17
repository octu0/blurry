package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func embossAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), "emboss", nil); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "emboss",
		Action: embossAction,
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
		},
	})
}
