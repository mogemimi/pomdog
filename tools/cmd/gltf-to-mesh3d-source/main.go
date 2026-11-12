// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"
	"path/filepath"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
)

type options struct {
	InFile  string
	OutFile string
}

func main() {
	opts := &options{}
	flag.StringVar(&opts.InFile, "i", "", "input glTF binary file (*.glb, required)")
	flag.StringVar(&opts.OutFile, "o", "", "output mesh source file (*.mesh3dsource, required)")
	flag.Parse()

	if opts.InFile == "" {
		log.Fatal("-i is required")
	}
	if opts.OutFile == "" {
		log.Fatal("-o is required")
	}

	if err := run(opts); err != nil {
		log.Fatal(err)
	}
}

func run(opts *options) error {
	data, err := os.ReadFile(opts.InFile)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	jsonChunk, binChunk, err := parseGLB(data)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	doc, err := parseGLTFDocument(jsonChunk)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	source, err := convert(doc, binChunk)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	if err := os.MkdirAll(filepath.Dir(opts.OutFile), fs.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(opts.OutFile, packSource(source), fs.ModePerm); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}

func packSource(source *schemas.Mesh3DSourceT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(source.Pack(builder))
	return builder.FinishedBytes()
}
