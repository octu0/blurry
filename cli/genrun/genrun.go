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

var (
	benchmarkOpt bool = false
)

func useBenchmarkOpt() {
	benchmarkOpt = true
}

func generate(runtimePath, blurryPath, target string) (string, error) {
	realRuntimePath, err := filepath.Abs(runtimePath)
	if err != nil {
		return "", err
	}
	runtimePath = realRuntimePath

	blurryFileName := filepath.Base(blurryPath)
	blurryDirName := filepath.Dir(blurryPath)
	blurryExt := filepath.Ext(blurryFileName)
	targetFileSuffix := "_" + target + blurryExt
	blurryGenName := strings.Replace(blurryFileName, blurryExt, targetFileSuffix, strings.LastIndex(blurryFileName, blurryExt))
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

	benchmarkOptFlag := ""
	if benchmarkOpt {
		benchmarkOptFlag = "-O2"
	}

	// strip symbol
	genArgs := []string{
		"g++",
		"-g",
		benchmarkOptFlag,
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

func generateWithBenchmark(runtimePath, blurryPath string) (string, error) {
	return generate(runtimePath, blurryPath, "benchmark")
}

func generateWithJIT(runtimePath, blurryPath string) (string, error) {
	return generate(runtimePath, blurryPath, "jit")
}

type runArgs []string

func runLocalInputFiles(runtimePath, generateOutFilePath string, inputFiles []string, commandName string, args runArgs) error {
	inputs := make([]string, len(inputFiles))
	for i, f := range inputFiles {
		p, err := filepath.Abs(f)
		if err != nil {
			return err
		}
		inputs[i] = p
	}

	out, err := ioutil.TempFile("/tmp", "out*.png")
	if err != nil {
		return err
	}
	defer out.Close()

	p := append([]string{commandName}, inputs...)
	p = append(p, args...)
	p = append(p, out.Name())
	return runexec(runtimePath, generateOutFilePath, p)
}

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
	defer log.Printf("info: open %s", out.Name())

	p := []string{
		commandName,
		input,
	}
	p = append(p, args...)
	p = append(p, out.Name())

	return runexec(runtimePath, generateOutFilePath, p)
}

func runexec(runtimePath, generateOutFilePath string, args []string) error {
	cmd := exec.Command(generateOutFilePath, args...)
	cmd.Env = append(os.Environ(), "DYLD_LIBRARY_PATH="+runtimePath+"/lib")
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr

	log.Printf("debug: command: %v", cmd.Args)
	if err := cmd.Run(); err != nil {
		return err
	}
	return nil
}
