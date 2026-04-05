// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"fmt"
	"os"
	"os/exec"
	"strings"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "usage: fxc-wrapper <fxc.exe> [fxc options...]")
		os.Exit(1)
	}

	fxcExe := os.Args[1]
	fxcArgs := os.Args[2:]

	cmd := exec.Command(fxcExe, fxcArgs...)

	var stdout bytes.Buffer
	var stderr bytes.Buffer
	cmd.Stdout = &stdout
	cmd.Stderr = &stderr

	err := cmd.Run()

	// NOTE: Always forward stderr as-is (contains error messages).
	if stderr.Len() > 0 {
		os.Stderr.Write(stderr.Bytes())
	}

	// NOTE: Filter stdout: strip "compilation object save succeeded; see ..." lines.
	if stdout.Len() > 0 {
		for _, line := range strings.Split(stdout.String(), "\n") {
			trimmed := strings.TrimSpace(line)
			if trimmed == "" {
				continue
			}
			if strings.HasPrefix(trimmed, "compilation object save succeeded") {
				continue
			}
			fmt.Fprintln(os.Stdout, line)
		}
	}

	if err != nil {
		if exitErr, ok := err.(*exec.ExitError); ok {
			os.Exit(exitErr.ExitCode())
		}
		fmt.Fprintf(os.Stderr, "fxc-wrapper: %v\n", err)
		os.Exit(1)
	}
}
