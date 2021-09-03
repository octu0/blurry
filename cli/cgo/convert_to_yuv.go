package cgo

import (
	"fmt"
	"log"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func convertToYUVAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}
	if c.String("c") != "444" {
		return fmt.Errorf("supports only chroma sample 444 yet ('%s')", c.String("c"))
	}

	ycbcr, err := blurry.ConvertToYUV444(in)
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
				Value: "444",
			},
		},
	})
}
