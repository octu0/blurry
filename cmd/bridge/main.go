package main

import (
	"log"
	"os"

	"github.com/comail/colog"
	"gopkg.in/urfave/cli.v1"

	"github.com/octu0/blurry"
	"github.com/octu0/blurry/cli/bridge"
)

func main() {
	colog.SetDefaultLevel(colog.LDebug)
	colog.SetMinLevel(colog.LDebug)

	colog.SetFormatter(&colog.StdFormatter{
		Flag: log.Ldate | log.Ltime | log.Lshortfile,
	})
	colog.Register()

	app := cli.NewApp()
	app.Version = blurry.Version
	app.Name = blurry.AppName
	app.Usage = ""
	app.Commands = bridge.Command()
	app.Flags = []cli.Flag{
		cli.BoolFlag{
			Name:  "debug, d",
			Usage: "debug mode",
		},
		cli.BoolFlag{
			Name:  "verbose, V",
			Usage: "verbose. more message",
		},
	}
	if err := app.Run(os.Args); err != nil {
		log.Printf("error: %s", err.Error())
		cli.OsExiter(1)
	}
}
