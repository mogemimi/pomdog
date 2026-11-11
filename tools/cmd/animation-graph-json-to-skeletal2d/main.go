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
	InFile   string
	OutFile  string
	OutDebug string
}

func main() {
	opts := &options{}
	flag.StringVar(&opts.InFile, "i", "", "input animation graph JSON file (required)")
	flag.StringVar(&opts.OutFile, "o", "", "output animation graph file (*.animgraph_skel2d, required)")
	flag.StringVar(&opts.OutDebug, "out-debug", "", "output debug file (*.animgraph_skel2d-debug, optional)")
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

	doc, err := parseGraphJSON(data)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	out, err := convert(doc)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	if err := writeFile(opts.OutFile, packGraph(out.Graph)); err != nil {
		return err
	}

	// NOTE: Write debug binary (optional)
	if opts.OutDebug != "" {
		if err := writeFile(opts.OutDebug, packDebug(out.Debug)); err != nil {
			return err
		}
	}

	return nil
}

func packGraph(graph *schemas.Skeletal2DAnimGraphT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(graph.Pack(builder))
	return builder.FinishedBytes()
}

func packDebug(debug *schemas.Skeletal2DAnimGraphDebugT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(debug.Pack(builder))
	return builder.FinishedBytes()
}

func writeFile(path string, data []byte) error {
	if err := os.MkdirAll(filepath.Dir(path), fs.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(path, data, fs.ModePerm); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}
