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
	InFile      string
	OutSkeleton string
	OutSkin     string
	OutClip     string
	OutDebug    string
}

func main() {
	opts := &options{}
	flag.StringVar(&opts.InFile, "i", "", "input Spine JSON file (required)")
	flag.StringVar(&opts.OutSkeleton, "out-skeleton", "", "output skeleton file (*.skeleton_skel2d, required)")
	flag.StringVar(&opts.OutSkin, "out-skin", "", "output skin file (*.skin_skel2d, required)")
	flag.StringVar(&opts.OutClip, "out-clip", "", "output clip file (*.clip_skel2d, required)")
	flag.StringVar(&opts.OutDebug, "out-debug", "", "output debug file (*.skel2d-debug, optional)")
	flag.Parse()

	if opts.InFile == "" {
		log.Fatal("-i is required")
	}
	if opts.OutSkeleton == "" {
		log.Fatal("--out-skeleton is required")
	}
	if opts.OutSkin == "" {
		log.Fatal("--out-skin is required")
	}
	if opts.OutClip == "" {
		log.Fatal("--out-clip is required")
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

	doc, err := parseSpineJSON(data)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	out, err := convert(doc)
	if err != nil {
		return fmt.Errorf("%s: %w", opts.InFile, err)
	}

	if err := writeFile(opts.OutSkeleton, packSkeleton(out.Skeleton)); err != nil {
		return err
	}
	if err := writeFile(opts.OutSkin, packSkinSet(out.SkinSet)); err != nil {
		return err
	}
	if err := writeFile(opts.OutClip, packClipSet(out.ClipSet)); err != nil {
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

func packSkeleton(skeleton *schemas.Skeletal2DSkeletonT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(skeleton.Pack(builder))
	return builder.FinishedBytes()
}

func packSkinSet(skinSet *schemas.Skeletal2DSkinSetT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(skinSet.Pack(builder))
	return builder.FinishedBytes()
}

func packClipSet(clipSet *schemas.Skeletal2DClipSetT) []byte {
	builder := flatbuffers.NewBuilder(0)
	builder.Finish(clipSet.Pack(builder))
	return builder.FinishedBytes()
}

func packDebug(debug *schemas.Skeletal2DDebugT) []byte {
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
