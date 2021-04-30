package genrun

import (
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"gopkg.in/urfave/cli.v1"
)

var commands []cli.Command

func addCommand(cmd cli.Command) {
	commands = append(commands, cmd)
}

func Command() []cli.Command {
	return commands
}

func generate(runtimePath, blurryPath string) (string, error) {
	realRuntimePath, err := filepath.Abs(runtimePath)
	if err != nil {
		return "", err
	}
	runtimePath = realRuntimePath

	blurryFileName := filepath.Base(blurryPath)
	blurryDirName := filepath.Dir(blurryPath)
	blurryExt := filepath.Ext(blurryFileName)
	blurryGenName := strings.Replace(blurryFileName, blurryExt, "_gen"+blurryExt, strings.LastIndex(blurryFileName, blurryExt))
	blurryGenPath := filepath.Join(blurryDirName, blurryGenName)

	mktemp, err := exec.Command("mktemp", "/tmp/outXXXX").Output()
	if err != nil {
		return "", err
	}
	generateOutFilePath := strings.TrimSpace(string(mktemp))

	libpng := exec.Command("libpng-config", "--cflags", "--ldflags")
	libpngCfg, err := libpng.Output()
	if err != nil {
		return "", err
	}
	libpngFlags := strings.TrimSpace(string(libpngCfg))
	libpngFlags = strings.ReplaceAll(libpngFlags, "\n", " ")

	// strip symbol
	genArgs := []string{
		"g++",
		"-g",
		"-I" + runtimePath + "/include",
		"-I" + runtimePath + "/share/Halide/tools",
		"-L" + runtimePath + "/lib",
		libpngFlags,
		"-ljpeg",
		"-lHalide",
		"-lpthread",
		"-ldl",
		"-std=c++11",
		"-o", generateOutFilePath,
		blurryPath,
		blurryGenPath,
	}
	log.Printf("info: generate %v", genArgs)

	cmd := exec.Command("sh", "-c", strings.Join(genArgs, " "))
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		return "", err
	}
	return generateOutFilePath, nil
}

type runArgs []string

func runLocal(runtimePath, generateOutFilePath, inputFilePath string, commandName string, args runArgs) error {
	input, err := filepath.Abs(inputFilePath)
	if err != nil {
		return err
	}

	out, err := ioutil.TempFile("/tmp", "out*.png")
	if err != nil {
		return err
	}
	defer out.Close()

	p := []string{
		commandName,
		input,
	}
	p = append(p, args...)
	p = append(p, out.Name())

	cmd := exec.Command(generateOutFilePath, p...)
	cmd.Env = append(os.Environ(), "DYLD_LIBRARY_PATH="+runtimePath+"/lib")
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	log.Printf("debug: command: %v", cmd.Args)
	if err := cmd.Run(); err != nil {
		return err
	}
	log.Printf("info: open %s", out.Name())
	return nil
}
