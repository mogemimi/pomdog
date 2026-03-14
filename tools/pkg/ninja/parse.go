// Copyright mogemimi. Distributed under the MIT license.

package ninja

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// ParseBuildOutputs reads a ninja build file and returns all explicit and
// implicit output file paths declared in build statements.
func ParseBuildOutputs(filename string) ([]string, error) {
	f, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("os.Open() failed: %w", err)
	}
	defer f.Close()

	var outputs []string

	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := scanner.Text()

		// Join line continuations ($\n)
		for strings.HasSuffix(line, "$") {
			if !scanner.Scan() {
				break
			}
			line = line[:len(line)-1] + scanner.Text()
		}

		if !strings.HasPrefix(line, "build ") {
			continue
		}

		// NOTE: Format of build statement:
		// ```
		// build <outputs> [| <implicit_outputs>]: <rule> <inputs> ...
		// ```
		rest := line[len("build "):]

		colonIdx := strings.Index(rest, ": ")
		if colonIdx < 0 {
			continue
		}
		outputsPart := rest[:colonIdx]

		// NOTE: Split explicit and implicit outputs on " | "
		parts := strings.SplitN(outputsPart, " | ", 2)

		for _, part := range parts {
			for _, token := range strings.Fields(part) {
				path := unescapeNinjaPath(token)
				if path != "" {
					outputs = append(outputs, path)
				}
			}
		}
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("scanner error: %w", err)
	}

	return outputs, nil
}

func unescapeNinjaPath(s string) string {
	// NOTE: Unescape Ninja path
	s = strings.ReplaceAll(s, "$ ", " ")
	s = strings.ReplaceAll(s, "$:", ":")
	return s
}
