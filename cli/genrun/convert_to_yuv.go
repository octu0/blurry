package genrun

import (
	"io/ioutil"

	"gopkg.in/urfave/cli.v1"
)

func convertToYUVAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generate(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	outY, err := ioutil.TempFile("/tmp", "out_y_*.png")
	if err != nil {
		return err
	}
	defer outY.Close()

	outU, err := ioutil.TempFile("/tmp", "out_u_*.png")
	if err != nil {
		return err
	}
	defer outU.Close()

	outV, err := ioutil.TempFile("/tmp", "out_v_*.png")
	if err != nil {
		return err
	}
	defer outV.Close()

	args := []string{
		outY.Name(),
		outU.Name(),
		outV.Name(),
	}
	cmd := "convert_to_yuvi444"
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
		},
	})
}
