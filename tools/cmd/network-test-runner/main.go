// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"errors"
	"fmt"
	"os"
	"os/exec"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "usage: network-test-runner <test executable> [test options...]")
		os.Exit(1)
	}

	exitCode, err := run(os.Args[1], os.Args[2:])
	if err != nil {
		fmt.Fprintf(os.Stderr, "network-test-runner: %v\n", err)
	}
	os.Exit(exitCode)
}

// run starts the local servers, runs testExe with their addresses followed by
// testArgs, and stops the servers. It returns the exit code for the runner.
func run(testExe string, testArgs []string) (int, error) {
	s, err := startServers()
	if err != nil {
		return 1, err
	}
	defer s.close()

	cmd := exec.Command(testExe, append(s.options(), testArgs...)...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Run(); err != nil {
		// NOTE: ExitCode() is -1 when a signal ended the test executable.
		var exitErr *exec.ExitError
		if errors.As(err, &exitErr) && (exitErr.ExitCode() >= 0) {
			return exitErr.ExitCode(), nil
		}
		return 1, err
	}
	return 0, nil
}
