package genrun

import (
	"gopkg.in/urfave/cli.v1"
)

func convertYUVAction(c *cli.Context) error {
	runtimePath := c.String("runtime")
	generateOutFilePath, err := generate(runtimePath, c.String("file"))
	if err != nil {
		return err
	}

	in := []string{
		c.String("input-y"),
		c.String("input-u"),
		c.String("input-v"),
	}

	cmd := "convert_from_yuv" + c.String("c")
	if err := runLocalInputFiles(runtimePath, generateOutFilePath, in, cmd, nil); err != nil {
		return err
	}

	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "convert_from_yuv",
		Action: convertYUVAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "input-y",
				Usage: "/path/to/input image-y",
				Value: "./testdata/yuv_i420_y_320x240.raw",
			},
			cli.StringFlag{
				Name:  "input-u",
				Usage: "/path/to/input image-u",
				Value: "./testdata/yuv_i420_u_320x240.raw",
			},
			cli.StringFlag{
				Name:  "input-v",
				Usage: "/path/to/input image-v",
				Value: "./testdata/yuv_i420_v_320x240.raw",
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
