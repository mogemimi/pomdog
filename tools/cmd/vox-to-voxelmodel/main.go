// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/mogemimi/pomdog/tools/pkg/magicavoxel"
	"github.com/mogemimi/pomdog/tools/pkg/voxelmodel"
)

type options struct {
	InFile  string
	OutFile string
	Prune   pruneOptions
}

func main() {
	opts := &options{}
	flag.StringVar(&opts.InFile, "i", "", "input MagicaVoxel file (*.vox, required)")
	flag.StringVar(&opts.OutFile, "o", "", "output voxel model file (*.voxelmodel, required)")
	flag.BoolVar(&opts.Prune.DropUnusedMaterials, "drop-unused-materials", false, "drop the materials of the colors that no voxel uses")
	flag.BoolVar(&opts.Prune.DropUnusedLayers, "drop-unused-layers", false, "drop the layers that no transform uses")
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
	file, err := magicavoxel.Decode(data)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}
	model, err := convert(file)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}
	prune(model, opts.Prune)
	return voxelmodel.WriteFile(opts.OutFile, model)
}
