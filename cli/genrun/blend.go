package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func blendAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		c.String("target"),
	}
	cmd := "blend_" + c.String("mode")
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, args); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "blend",
		Action: blendAction,
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
				Name:  "t,target",
				Usage: "/path/to/input image",
				Value: "./testdata/src160x160.png",
			},
			cli.StringFlag{
				Name:  "m,mode",
				Usage: "blend mode ('normal' or 'sub', 'add', 'diff')",
				Value: "normal",
			},
		},
	})
}
