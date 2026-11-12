// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"
	"path/filepath"

	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
	"github.com/mogemimi/pomdog/tools/pkg/voxelmodel"
)

type options struct {
	InFile  string
	OutFile string
}

func main() {
	opts := &options{}
	flag.StringVar(&opts.InFile, "i", "", "input voxel model file (*.voxelmodel, required)")
	flag.StringVar(&opts.OutFile, "o", "", "output MagicaVoxel file (*.vox, required)")
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
	model, err := voxelmodel.ReadFile(opts.InFile)
	if err != nil {
		return err
	}
	data, err := magicavoxel.Encode(convert(model))
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}
	if err := os.MkdirAll(filepath.Dir(opts.OutFile), fs.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(opts.OutFile, data, fs.ModePerm); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}
