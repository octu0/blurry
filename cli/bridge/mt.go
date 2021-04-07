package bridge

import (
	"log"
	"sort"

	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
)

func matchTemplateAction(c *cli.Context) error {
	in, err := loadImage(c.String("input"))
	if err != nil {
		return err
	}
	tpl, err := loadImage(c.String("template"))
	if err != nil {
		return err
	}

	var scores []blurry.MatchTemplateScore
	switch c.String("mode") {
	case "sad":
		sadScores, err := blurry.MatchTemplateSAD(in, tpl, uint16(c.Int("threshold")))
		if err != nil {
			return err
		}
		scores = sadScores
	case "ssd":
		sadScores, err := blurry.MatchTemplateSSD(in, tpl, uint32(c.Int("threshold")))
		if err != nil {
			return err
		}
		scores = sadScores
	}

	sort.Slice(scores, func(i, j int) bool {
		return scores[i].Score < scores[j].Score
	})

	if 10 < len(scores) {
		scores = scores[0:10]
	}

	for _, s := range scores {
		log.Printf("info: match x:%d y:%d score:%d", s.Point.X, s.Point.Y, s.Score)
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
				Usage: "matching method: sad,ssd",
				Value: "sad",
			},
			cli.BoolFlag{
				Name:  "r,render",
				Usage: "render match image",
			},
			cli.IntFlag{
				Name:  "threshold",
				Usage: "filter threshold",
				Value: 1000,
			},
		},
	})
}
