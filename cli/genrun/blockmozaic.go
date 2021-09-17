package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func blockmozaicAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		c.String("block"),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), "blockmozaic", runArgs(args)); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "blockmozaic",
		Action: blockmozaicAction,
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
				Name:  "block",
				Usage: "block size",
				Value: "10",
			},
		},
	})
}
