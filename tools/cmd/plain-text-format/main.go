// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"strings"

	"golang.org/x/sync/errgroup"
)

func main() {
	defaultJobs := runtime.NumCPU()

	var options struct {
		Jobs                   int
		ReplaceCRLF            bool
		RemoveSpaceAtEndOfLine bool
		RemoveNewlineFromEOF   bool
		RemoveBOM              bool
		ReplaceTabsWithSpaces  bool
	}
	flag.IntVar(&options.Jobs, "jobs", defaultJobs, fmt.Sprintf("run N jobs in parallel [default=%d on this system]", defaultJobs))
	flag.BoolVar(&options.ReplaceCRLF, "replace-crlf", false, "replace crlf")
	flag.BoolVar(&options.RemoveSpaceAtEndOfLine, "remove-space-eol", false, "remove space at end of line")
	flag.BoolVar(&options.RemoveNewlineFromEOF, "remove-newline-eof", false, "remove newline from end of file")
	flag.BoolVar(&options.RemoveBOM, "remove-bom", false, "remove UTF-8 BOM")
	flag.BoolVar(&options.ReplaceTabsWithSpaces, "replace-tabs", false, "replace hard tabs with 4 spaces")
	flag.Parse()

	patterns := flag.Args()

	files := []string{}
	for _, p := range patterns {
		matches, err := filepath.Glob(p)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
		files = append(files, matches...)
	}

	formatOptions := FormatOptions{
		ReplaceCRLF:            options.ReplaceCRLF,
		RemoveSpaceAtEndOfLine: options.RemoveSpaceAtEndOfLine,
		RemoveNewlineFromEOF:   options.RemoveNewlineFromEOF,
		RemoveBOM:              options.RemoveBOM,
		ReplaceTabsWithSpaces:  options.ReplaceTabsWithSpaces,
	}

	var eg errgroup.Group
	eg.SetLimit(options.Jobs)

	for _, f := range files {
		file := f
		eg.Go(func() error {
			if err := processFile(file, formatOptions); err != nil {
				return fmt.Errorf("processFile(file) failed at %s: %w", file, err)
			}
			return nil
		})
	}

	if err := eg.Wait(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}

func processFile(path string, options FormatOptions) error {
	originalBytes, err := os.ReadFile(path)
	if err != nil {
		return fmt.Errorf("failed to read file: %w", err)
	}
	originalText := string(originalBytes)

	normalizedText := normalizeText(originalText, options)

	if originalText == normalizedText {
		// NOTE: No changes detected. Skipping save.
		return nil
	}

	err = os.WriteFile(path, []byte(normalizedText), os.ModePerm)
	if err != nil {
		return fmt.Errorf("failed to write file: %w", err)
	}
	return nil
}

type FormatOptions struct {
	ReplaceCRLF            bool
	RemoveSpaceAtEndOfLine bool
	RemoveNewlineFromEOF   bool
	RemoveBOM              bool
	ReplaceTabsWithSpaces  bool
}

func normalizeText(input string, options FormatOptions) string {
	if options.RemoveBOM {
		input = strings.TrimPrefix(input, "\xef\xbb\xbf")
	}

	if options.ReplaceCRLF {
		input = strings.ReplaceAll(input, "\r\n", "\n")
	}

	if options.ReplaceTabsWithSpaces {
		input = strings.ReplaceAll(input, "\t", "    ")
	}

	if options.RemoveSpaceAtEndOfLine {
		lines := strings.Split(input, "\n")
		for i := range lines {
			lines[i] = strings.TrimRight(lines[i], " \t")
		}
		input = strings.Join(lines, "\n")
	}

	if options.RemoveNewlineFromEOF {
		input = strings.TrimRight(input, "\n")

		if len(input) > 0 {
			input = input + "\n"
		}
	}

	return input
}
