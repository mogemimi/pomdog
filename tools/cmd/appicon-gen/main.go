// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"image"
	"image/png"
	"os"
	"strconv"
	"strings"

	"github.com/mogemimi/pomdog/tools/pkg/ico"
	"golang.org/x/image/draw"
)

type Options struct {
	OutFile    string
	InFile     string
	Format     string
	Sizes      string
	OutWidth   int
	OutHeight  int
	ResizeFunc string
}

func main() {
	var options Options
	flag.StringVar(&options.OutFile, "o", "", "output file (*.png or *.ico)")
	flag.StringVar(&options.InFile, "in", "", "input file (*.png)")
	flag.IntVar(&options.OutWidth, "w", 0, "output width for resizing")
	flag.IntVar(&options.OutHeight, "h", 0, "output height for resizing")
	flag.StringVar(&options.Format, "format", "png", "output file format (png, ico)")
	flag.StringVar(&options.Sizes, "sizes", "", "comma-separated list of sizes for multi-size ICO output (e.g. 16,32,48,256); takes precedence over -w/-h when --format ico")
	flag.StringVar(&options.ResizeFunc, "func", "CatmullRom", "resampling filter (CatmullRom, BiLinear, ApproxBiLinear, NearestNeighbor)")
	flag.Parse()

	if err := run(&options); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}

func run(options *Options) error {
	inFile, err := os.Open(options.InFile)
	if err != nil {
		return fmt.Errorf("failed to open input file: %w", err)
	}
	defer inFile.Close()

	imgSrc, _, err := image.Decode(inFile)
	if err != nil {
		return fmt.Errorf("failed to decode input file: %w", err)
	}

	// NOTE: Multi-size ICO path
	if options.Format == "ico" && options.Sizes != "" {
		sizes, err := parseSizes(options.Sizes)
		if err != nil {
			return fmt.Errorf("failed to parse sizes: %w", err)
		}

		outFile, err := os.Create(options.OutFile)
		if err != nil {
			return fmt.Errorf("failed to create output file: %w", err)
		}
		defer outFile.Close()

		if err := ico.EncodeMulti(outFile, imgSrc, sizes); err != nil {
			return fmt.Errorf("failed to encode multi-size ICO: %w", err)
		}
		return nil
	}

	// NOTE: Single-image path
	bounds := imgSrc.Bounds()

	if options.OutWidth == 0 {
		options.OutWidth = bounds.Dx()
	}
	if options.OutHeight == 0 {
		options.OutHeight = bounds.Dy()
	}

	if options.OutWidth <= 0 {
		return fmt.Errorf("output width must be > 0")
	}
	if options.OutHeight <= 0 {
		return fmt.Errorf("output height must be > 0")
	}

	imgDst := image.NewRGBA(image.Rect(0, 0, options.OutWidth, options.OutHeight))

	switch options.ResizeFunc {
	case "CatmullRom":
		draw.CatmullRom.Scale(imgDst, imgDst.Bounds(), imgSrc, bounds, draw.Over, nil)

	case "BiLinear":
		draw.BiLinear.Scale(imgDst, imgDst.Bounds(), imgSrc, bounds, draw.Over, nil)

	case "ApproxBiLinear":
		draw.ApproxBiLinear.Scale(imgDst, imgDst.Bounds(), imgSrc, bounds, draw.Over, nil)

	case "NearestNeighbor":
		draw.NearestNeighbor.Scale(imgDst, imgDst.Bounds(), imgSrc, bounds, draw.Over, nil)

	default:
		return fmt.Errorf("unknown resampling filter: %s", options.ResizeFunc)
	}

	switch options.Format {
	case "png":
		outFile, err := os.Create(options.OutFile)
		if err != nil {
			return fmt.Errorf("failed to create output file: %w", err)
		}
		defer outFile.Close()

		if err := png.Encode(outFile, imgDst); err != nil {
			return fmt.Errorf("failed to encode PNG: %w", err)
		}

	case "ico":
		outFile, err := os.Create(options.OutFile)
		if err != nil {
			return fmt.Errorf("failed to create output file: %w", err)
		}
		defer outFile.Close()

		if err := ico.Encode(outFile, imgDst); err != nil {
			return fmt.Errorf("failed to encode ICO: %w", err)
		}

	default:
		return fmt.Errorf("unknown output format: %s", options.Format)
	}
	return nil
}

func parseSizes(s string) ([]int, error) {
	parts := strings.Split(s, ",")
	sizes := make([]int, 0, len(parts))
	for _, p := range parts {
		p = strings.TrimSpace(p)
		n, err := strconv.Atoi(p)
		if err != nil {
			return nil, fmt.Errorf("invalid size %q: %w", p, err)
		}
		if n <= 0 {
			return nil, fmt.Errorf("size must be > 0, got %d", n)
		}
		sizes = append(sizes, n)
	}
	return sizes, nil
}
