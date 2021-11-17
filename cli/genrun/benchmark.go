package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func benchmarkAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	//useBenchmarkOpt()
	generateOutFilePath, err := generateWithBenchmark(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	args := []string{
		c.String("template"),
	}
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), "benchmark", args); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "benchmark",
		Action: benchmarkAction,
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
		},
	})
}
