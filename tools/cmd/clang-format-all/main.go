// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"flag"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"

	"golang.org/x/sync/errgroup"
)

func main() {
	defaultJobs := runtime.NumCPU()

	var options struct {
		Jobs        int
		ClangFormat string
	}
	flag.IntVar(&options.Jobs, "jobs", defaultJobs, fmt.Sprintf("run N jobs in parallel [default=%d on this system]", defaultJobs))
	flag.StringVar(&options.ClangFormat, "clangformat", "", "path to clang-format executable")
	flag.Parse()

	patterns := flag.Args()

	if len(options.ClangFormat) == 0 {
		searchPaths := []string{
			"clang-format",
		}

		if runtime.GOOS == "windows" {
			searchPaths = append(searchPaths, "${programfiles}/LLVM/bin/clang-format.exe")
			searchPaths = append(searchPaths, "${programfiles}/Microsoft Visual Studio/18/Professional/VC/Tools/Llvm/bin/clang-format.exe")
			searchPaths = append(searchPaths, "${programfiles}/Microsoft Visual Studio/18/Community/VC/Tools/Llvm/bin/clang-format.exe")
			searchPaths = append(searchPaths, "${programfiles}/Microsoft Visual Studio/2022/Professional/VC/Tools/Llvm/bin/clang-format.exe")
			searchPaths = append(searchPaths, "${programfiles}/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang-format.exe")
			searchPaths = append(searchPaths, "${programfiles}/Microsoft Visual Studio/2019/Professional/VC/Tools/Llvm/bin/clang-format.exe")
			searchPaths = append(searchPaths, "${programfiles}/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/clang-format.exe")
		}

		for _, s := range searchPaths {
			exe := filepath.Clean(os.ExpandEnv(s))

			cmd := exec.Command(exe, "--version")
			var stdout bytes.Buffer
			var stderr bytes.Buffer
			cmd.Stdout = &stdout
			cmd.Stderr = &stderr
			if err := cmd.Run(); err == nil && len(stderr.Bytes()) == 0 {
				options.ClangFormat = exe
				break
			}
		}

		if len(options.ClangFormat) == 0 {
			fmt.Fprintln(os.Stderr, "clang-format: command not found")
			os.Exit(1)
		}
	}

	files := []string{}
	for _, p := range patterns {
		matches, err := filepath.Glob(p)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
		files = append(files, matches...)
	}

	var eg errgroup.Group
	eg.SetLimit(options.Jobs)

	for _, f := range files {
		file := f
		eg.Go(func() error {
			cmd := exec.Command(options.ClangFormat, "-i", file)
			cmd.Stdout = os.Stdout
			cmd.Stderr = os.Stderr
			if err := cmd.Run(); err != nil {
				return fmt.Errorf("exec.Command() failed at %s: %w", file, err)
			}
			return nil
		})
	}

	if err := eg.Wait(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
