package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func convertAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	cmd := "convert" + c.String("mode")
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, nil); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "convert",
		Action: convertAction,
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
				Name:  "mode",
				Usage: "convert from image layout(bgra)",
				Value: "bgra",
			},
		},
	})
}
