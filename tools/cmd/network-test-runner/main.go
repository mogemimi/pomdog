// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"errors"
	"fmt"
	"os"
	"os/exec"
	"slices"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintln(os.Stderr, "usage: network-test-runner <command> [arguments...]")
		os.Exit(1)
	}

	exitCode, err := run(os.Args[1], os.Args[2:])
	if err != nil {
		fmt.Fprintf(os.Stderr, "network-test-runner: %v\n", err)
	}
	os.Exit(exitCode)
}

// run starts the local servers, runs command with args followed by the server
// options, and stops the servers. It returns the exit code for the runner.
//
// The options come last, so that command can be an interpreter, such as
// Node.js running the web build of the tests, whose own options come first.
func run(command string, args []string) (int, error) {
	s, err := startServers()
	if err != nil {
		return 1, err
	}
	defer s.close()

	cmd := exec.Command(command, append(slices.Clip(args), s.options()...)...)

	// NOTE: Node.js trusts the root of the local servers through this
	// variable. Other test executables ignore it.
	cmd.Env = append(os.Environ(), "NODE_EXTRA_CA_CERTS="+s.caFile)
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
