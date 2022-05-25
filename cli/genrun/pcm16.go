package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func pcm16Action(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	var cmd string
	switch c.String("mode") {
	case "decibel":
		cmd = "pcm16_decibel"
	}

	args := []string{
		cmd,
		c.String("input"),
		c.String("length"),
	}
	if err := runexec(runtimePath, generateOutFilePath, args); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "pcm16",
		Action: pcm16Action,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input data",
				Value: "./testdata/pcm16_1.data",
			},
			cli.StringFlag{
				Name:  "l,length",
				Usage: "pcm16 frame length",
				Value: "1024",
			},
			cli.StringFlag{
				Name:  "m,mode",
				Usage: "mode string ('decibel')",
				Value: "decibel",
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
