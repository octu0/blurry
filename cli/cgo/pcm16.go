package cgo

import (
	"encoding/binary"
	"log"
	"os"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func pcm16Action(c *cli.Context) error {
	switch c.String("mode") {
	case "decibel":
		data, err := os.ReadFile(c.String("input"))
		if err != nil {
			return err
		}

		decibel, err := blurry.PCM16Decibel(data, c.Int("length"))
		if err != nil {
			return err
		}
		log.Printf("info: %3.4fdB", decibel)
	case "decibel_native":
		f, err := os.Open(c.String("input"))
		if err != nil {
			return err
		}

		pcm := make([]int16, c.Int("length"))
		for i := 0; i < cap(pcm); i += 1 {
			if err := binary.Read(f, binary.LittleEndian, &pcm[i]); err != nil {
				return err
			}
		}

		decibel := blurry.NativePCM16Decibel(pcm)
		if err != nil {
			return err
		}
		log.Printf("info: %3.4fdB", decibel)
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
			cli.IntFlag{
				Name:  "l,length",
				Usage: "pcm16 frame length",
				Value: 1024,
			},
			cli.StringFlag{
				Name:  "m,mode",
				Usage: "mode string ('decibel')",
				Value: "decibel",
			},
		},
	})
}
