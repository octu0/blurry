package bridge

import (
	"image"
	"image/color"
	"log"
	"sort"
	"strconv"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func logIntScore(scores []blurry.MatchTemplateIntScore, in, tpl *image.RGBA, withRender bool) error {
	sort.Slice(scores, func(i, j int) bool {
		return scores[i].Score < scores[j].Score // asc, best match is nearest to 0
	})

	if 10 < len(scores) {
		scores = scores[0:10]
	}

	for _, s := range scores {
		log.Printf("info: match x:%d y:%d score:%d", s.Point.X, s.Point.Y, s.Score)
	}

	if withRender {
		out := image.NewRGBA(in.Rect)
		copy(out.Pix, in.Pix)

		tplBounds := tpl.Bounds()
		tplWidth := tplBounds.Max.X
		tplHeight := tplBounds.Max.Y

		for _, s := range scores {
			startX, endX := s.Point.X, s.Point.X+tplWidth
			startY, endY := s.Point.Y, s.Point.Y+tplHeight
			renderBox(out, startX, endX, startY, endY, color.RGBA{R: 255, G: 0, B: 0, A: 255})
		}
		path, err := saveImage(out)
		if err != nil {
			return err
		}
		log.Printf("info: open %s", path)
	}
	return nil
}

func logFloatScore(scores []blurry.MatchTemplateFloatScore, in, tpl *image.RGBA, withRender bool) error {
	sort.Slice(scores, func(i, j int) bool {
		return scores[i].Score > scores[j].Score // desc, best match is nearest to 1.0
	})

	if 10 < len(scores) {
		scores = scores[0:10]
	}

	for _, s := range scores {
		log.Printf("info: match x:%d y:%d score:%2.5f", s.Point.X, s.Point.Y, s.Score)
	}

	if withRender {
		out := image.NewRGBA(in.Rect)
		copy(out.Pix, in.Pix)

		tplBounds := tpl.Bounds()
		tplWidth := tplBounds.Max.X
		tplHeight := tplBounds.Max.Y

		for _, s := range scores {
			startX, endX := s.Point.X, s.Point.X+tplWidth
			startY, endY := s.Point.Y, s.Point.Y+tplHeight
			renderBox(out, startX, endX, startY, endY, color.RGBA{R: 255, G: 0, B: 0, A: 255})
		}
		path, err := saveImage(out)
		if err != nil {
			return err
		}
		log.Printf("info: open %s", path)
	}
	return nil
}

func matchTemplateAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}
	tpl, err := loadImage(c.String("template"))
	if err != nil {
		return err
	}

	threshold := c.String("threshold")
	if threshold == "" {
		switch c.String("mode") {
		case "sad", "ssd":
			log.Printf("error: --threshold must be specified, value is integer")
			return nil
		case "ncc":
			log.Printf("error: --threshold must be specified, value is float")
			return nil
		}
	}

	switch c.String("mode") {
	case "sad":
		th, err := strconv.ParseUint(threshold, 10, 32)
		if err != nil {
			return err
		}
		sadScores, err := blurry.MatchTemplateSAD(in, tpl, uint16(th))
		if err != nil {
			return err
		}
		return logIntScore(sadScores, in, tpl, c.Bool("render"))
	case "ssd":
		th, err := strconv.ParseUint(threshold, 10, 32)
		if err != nil {
			return err
		}
		ssdScores, err := blurry.MatchTemplateSSD(in, tpl, uint32(th))
		if err != nil {
			return err
		}
		return logIntScore(ssdScores, in, tpl, c.Bool("render"))
	case "ncc":
		th, err := strconv.ParseFloat(threshold, 32)
		if err != nil {
			return err
		}
		nccScores, err := blurry.MatchTemplateNCC(in, tpl, float32(th))
		if err != nil {
			return err
		}
		return logFloatScore(nccScores, in, tpl, c.Bool("render"))
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
				Name:  "t,template",
				Usage: "/path/to/input image",
				Value: "./testdata/tpl.png",
			},
			cli.StringFlag{
				Name:  "m,mode",
				Usage: "matching method: sad,ssd,ncc",
				Value: "sad",
			},
			cli.BoolFlag{
				Name:  "r,render",
				Usage: "render match image",
			},
			cli.StringFlag{
				Name:  "threshold",
				Usage: "filter threshold(sad and ssd to integer value, ncc float value)",
				Value: "",
			},
		},
	})
}
