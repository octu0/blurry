package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func cannyAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		c.String("max"),
		c.String("min"),
	}

	var cmd string = "canny"

	if c.String("dilate") != "0" {
		switch c.String("mode") {
		case "0":
			cmd = "canny_dilate"
			args = append(args, c.String("dilate"))
		case "1":
			cmd = "canny_morphology_open"
			args = append(args, c.String("size"))
			args = append(args, c.String("dilate"))
		case "2":
			cmd = "canny_morphology_close"
			args = append(args, c.String("size"))
			args = append(args, c.String("dilate"))
		}
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, args); err != nil {
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
				Name:  "m,mode",
				Usage: "morphology mode (0=none, 1=open, 2=close)",
				Value: "0",
			},
			cli.StringFlag{
				Name:  "s,size",
				Usage: "morphology size (0 is skip morphology)",
				Value: "0",
			},
			cli.StringFlag{
				Name:  "d,dilate",
				Usage: "dilate value(0 is no dilation)",
				Value: "0",
			},
		},
	})
}
