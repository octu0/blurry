package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func flipAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	flipMode := ""
	switch c.String("flip") {
	case "v", "vertical":
		flipMode = "V"
	case "h", "horizon":
		flipMode = "H"
	}
	cmd := "flip" + flipMode
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, nil); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "flip",
		Action: flipAction,
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
				Name:  "flip",
				Usage: "flip direction(v,vertical,h,horizon)",
				Value: "v",
			},
		},
	})
}
