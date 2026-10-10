// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"errors"
	"fmt"
	"os"
	"os/exec"
	"time"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "usage: zstd-wrapper <zstd> [zstd options...] -o <output> [inputs...]")
		os.Exit(1)
	}

	exitCode, err := run(os.Args[1], os.Args[2:])
	if err != nil {
		fmt.Fprintf(os.Stderr, "zstd-wrapper: %v\n", err)
	}
	os.Exit(exitCode)
}

// outputFile returns the file that follows the only `-o` option in args.
func outputFile(args []string) (string, error) {
	output := ""
	for i := 0; i < len(args); i++ {
		if args[i] != "-o" {
			continue
		}
		if i+1 >= len(args) {
			return "", errors.New("-o has no file")
		}
		if output != "" {
			return "", errors.New("-o is given more than once")
		}
		output = args[i+1]
		i++
	}
	if output == "" {
		return "", errors.New("-o is required")
	}
	return output, nil
}

// run runs zstdExe with args and gives the output file the current time as its
// modification time. It returns the exit code for the wrapper.
func run(zstdExe string, args []string) (int, error) {
	output, err := outputFile(args)
	if err != nil {
		return 1, err
	}

	cmd := exec.Command(zstdExe, args...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Run(); err != nil {
		var exitErr *exec.ExitError
		if errors.As(err, &exitErr) {
			return exitErr.ExitCode(), nil
		}
		return 1, err
	}

	// NOTE: zstd gives its output file the modification time of the input file,
	// so the output would look older than its inputs to build tools such as
	// Ninja. Touch it, as if zstd had written it now.
	now := time.Now()
	if err := os.Chtimes(output, now, now); err != nil {
		return 1, fmt.Errorf("os.Chtimes() failed: %w", err)
	}
	return 0, nil
}
