// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"

	ninja "github.com/mogemimi/pomdog/tools/pkg/ninja"
)

func main() {
	outFile := ""
	flag.StringVar(&outFile, "o", "", "output ninja file (build.ninja)")
	flag.Parse()

	subninjas := flag.Args()

	if outFile == "" {
		log.Fatal("-o is required")
	}
	if len(subninjas) == 0 {
		log.Fatal("at least one subninja file must be specified as a positional argument")
	}

	if err := run(outFile, subninjas); err != nil {
		log.Fatal(err)
	}
}

func run(outFile string, subninjas []string) error {
	if abs, err := filepath.Abs(outFile); err == nil {
		outFile = filepath.Clean(abs)
	}

	gen := &ninja.Generator{}

	for _, path := range subninjas {
		if abs, err := filepath.Abs(path); err == nil {
			path = filepath.Clean(abs)
		}
		gen.AddSubninja(path)
	}

	// NOTE: Create directory for output file
	if dir := filepath.Dir(outFile); dir != "" {
		if err := os.MkdirAll(dir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	content := gen.Generate()

	f, err := os.Create(outFile)
	if err != nil {
		return fmt.Errorf("os.Create() failed: %w", err)
	}
	defer f.Close()

	if _, err := f.WriteString(content); err != nil {
		return fmt.Errorf("f.WriteString() failed: %w", err)
	}

	return nil
}
