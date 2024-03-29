package genrun

import (
	"io/ioutil"

	"gopkg.in/urfave/cli.v1"
)

func convertToYUVAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generateWithJIT(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	outY, err := ioutil.TempFile("/tmp", "out_y_*.raw")
	if err != nil {
		return err
	}
	defer outY.Close()

	outU, err := ioutil.TempFile("/tmp", "out_u_*.raw")
	if err != nil {
		return err
	}
	defer outU.Close()

	outV, err := ioutil.TempFile("/tmp", "out_v_*.raw")
	if err != nil {
		return err
	}
	defer outV.Close()

	args := []string{
		outY.Name(),
		outU.Name(),
		outV.Name(),
	}
	cmd := "convert_to_yuv" + c.String("c")
	if err := runLocal(runtimePath, generateOutFilePath, c.String("input"), cmd, args); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "convert_to_yuv",
		Action: convertToYUVAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "input",
				Usage: "/path/to/input",
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
				Name:  "c",
				Usage: "chroma sampling",
				Value: "420",
			},
		},
	})
}
