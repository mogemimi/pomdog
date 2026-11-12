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
	"github.com/mogemimi/pomdog/tools/pkg/voxelmodel"
)

type options struct {
	InFile  string
	OutFile string
	Mesher  string
}

func main() {
	opts := &options{}
	flag.StringVar(&opts.InFile, "i", "", "input voxel model file (*.voxelmodel, required)")
	flag.StringVar(&opts.OutFile, "o", "", "output mesh source file (*.mesh3dsource, required)")
	flag.StringVar(&opts.Mesher, "mesher", "greedy", "faces to make: naive (every face), culled (faces that no voxel covers), or greedy (culled faces merged by color)")
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
	ms, err := parseMesher(opts.Mesher)
	if err != nil {
		return err
	}
	model, err := voxelmodel.ReadFile(opts.InFile)
	if err != nil {
		return err
	}

	builder := flatbuffers.NewBuilder(0)
	builder.Finish(convert(model, ms).Pack(builder))

	if err := os.MkdirAll(filepath.Dir(opts.OutFile), fs.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(opts.OutFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}
