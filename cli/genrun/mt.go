package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func matchTemplateAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	cmd := "match_template_" + c.String("mode")
	if c.Bool("prepared") {
		cmd = "prepared_" + cmd
	}
	args := []string{
		c.String("template"),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, args); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "match_template",
		Action: matchTemplateAction,
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
				Name:  "t,template",
				Usage: "/path/to/input image",
				Value: "./testdata/tpl.png",
			},
			cli.StringFlag{
				Name:  "m,mode",
				Usage: "matching method: sad,ssd,ncc,zncc",
				Value: "sad",
			},
			cli.BoolFlag{
				Name:  "p,prepared",
				Usage: "use NCC or ZNCC prepared template",
			},
		},
	})
}
