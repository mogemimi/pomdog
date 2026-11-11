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
	flag.StringVar(&opts.InFile, "i", "", "input particle clip JSON file (required)")
	flag.StringVar(&opts.OutFile, "o", "", "output particle clip file (*.particlesclip, required)")
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

	src, err := parseClipJSON(data)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	clip, err := convert(src)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	if err := os.MkdirAll(filepath.Dir(opts.OutFile), fs.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(opts.OutFile, packClip(clip), fs.ModePerm); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}

func packClip(clip *schemas.ParticlesClipT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(clip.Pack(builder))
	return builder.FinishedBytes()
}
