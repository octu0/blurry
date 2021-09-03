package cgo

import (
	"fmt"
	"image"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func convertFromYUV(cs string, y, u, v []byte, width, height int) (*image.RGBA, error) {
	r := image.Rect(0, 0, width, height)

	switch cs {
	case "420":
		return blurry.ConvertFromYUV420(&image.YCbCr{
			Y:              y,
			Cb:             u,
			Cr:             v,
			Rect:           r,
			YStride:        width,
			CStride:        width / 2,
			SubsampleRatio: image.YCbCrSubsampleRatio420,
		})
	case "444":
		return blurry.ConvertFromYUV444(&image.YCbCr{
			Y:              y,
			Cb:             u,
			Cr:             v,
			Rect:           r,
			YStride:        width,
			CStride:        width,
			SubsampleRatio: image.YCbCrSubsampleRatio444,
		})
	}
	return nil, fmt.Errorf("unknown chroma subsample: '%s'", cs)
}

func convertFromYUVAction(c *cli.Context) error {
	dataY, err := loadData(c.String("input-y"))
	if err != nil {
		return err
	}
	dataU, err := loadData(c.String("input-u"))
	if err != nil {
		return err
	}
	dataV, err := loadData(c.String("input-v"))
	if err != nil {
		return err
	}

	out, err := convertFromYUV(c.String("chroma"), dataY, dataU, dataV, c.Int("width"), c.Int("height"))
	if err != nil {
		return err
	}

	path, err := saveImage(out)
	if err != nil {
		return err
	}
	log.Printf("info: open %s", path)
	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "convert_from_yuv",
		Action: convertFromYUVAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "input-y",
				Usage: "/path/to/input y plane",
				Value: "./testdata/yuv_420_y_320x240.raw",
			},
			cli.StringFlag{
				Name:  "input-u",
				Usage: "/path/to/input u plane",
				Value: "./testdata/yuv_420_u_320x240.raw",
			},
			cli.StringFlag{
				Name:  "input-v",
				Usage: "/path/to/input v plane",
				Value: "./testdata/yuv_420_v_320x240.raw",
			},
			cli.IntFlag{
				Name:  "width",
				Usage: "image width",
				Value: 320,
			},
			cli.IntFlag{
				Name:  "height",
				Usage: "image height",
				Value: 240,
			},
			cli.StringFlag{
				Name:  "c,chroma",
				Usage: "from subsampling(420,444)",
				Value: "420",
			},
		},
	})
}
