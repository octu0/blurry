package cgo

import (
	"fmt"
	"image"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func convertToYUV(cs string, img *image.RGBA) (*image.YCbCr, error) {
	switch cs {
	case "420":
		return blurry.ConvertToYUV420(img)
	case "444":
		return blurry.ConvertToYUV444(img)
	}
	return nil, fmt.Errorf("unknown chroma subsample: '%s'", cs)
}

func convertToYUVAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}

	ycbcr, err := convertToYUV(c.String("c"), in)
	if err != nil {
		return err
	}

	log.Printf("info: yuv Y:%d U:%d V:%d YStride:%d CStride:%d", len(ycbcr.Y), len(ycbcr.Cb), len(ycbcr.Cr), ycbcr.YStride, ycbcr.CStride)

	pathY, err := saveData(ycbcr.Y)
	if err != nil {
		return err
	}
	pathU, err := saveData(ycbcr.Cb)
	if err != nil {
		return err
	}
	pathV, err := saveData(ycbcr.Cr)
	if err != nil {
		return err
	}

	log.Printf("info: y: %s", pathY)
	log.Printf("info: u: %s", pathU)
	log.Printf("info: v: %s", pathV)

	imgPath, err := saveImageImage(ycbcr)
	if err != nil {
		return err
	}
	log.Printf("info: image: %s", imgPath)
	return nil
}

func init() {
	addCommand(cli.Command{
		Name:   "convert_to_yuv",
		Action: convertToYUVAction,
		Flags: []cli.Flag{
			cli.StringFlag{
				Name:  "i,input",
				Usage: "/path/to/input image",
				Value: "./testdata/src.png",
			},
			cli.StringFlag{
				Name:  "c,chroma",
				Usage: "to subsampling(444)",
				Value: "420",
			},
		},
	})
}
