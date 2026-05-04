// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"flag"
	"fmt"
	"image"
	"image/png"
	"io/fs"
	"log"
	"os"
	"path/filepath"
	"sort"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

type convertOptions struct {
	outWidth           int
	outHeight          int
	perTileSpacing     int
	tightPacking       bool
	premultipliedAlpha bool
}

func main() {
	var rawOpts struct {
		InAtlas               string
		InTex                 string
		OutFile               string
		OutTex                string
		OutWidth              int
		OutHeight             int
		OutPerTileSpacing     int
		OutTightPacking       bool
		OutPremultipliedAlpha bool
	}
	flag.StringVar(&rawOpts.InAtlas, "in-atlas", "", "input .atlas file (required)")
	flag.StringVar(&rawOpts.InTex, "in-tex", "", "input PNG texture file (required)")
	flag.StringVar(&rawOpts.OutFile, "o", "", "output .tileset file (required)")
	flag.StringVar(&rawOpts.OutTex, "out-tex", "", "output PNG texture file (required)")
	flag.IntVar(&rawOpts.OutWidth, "out-width", 0, "output texture width in pixels (default: same as input)")
	flag.IntVar(&rawOpts.OutHeight, "out-height", 0, "output texture height in pixels (default: same as input)")
	flag.IntVar(&rawOpts.OutPerTileSpacing, "out-per-tile-spacing", 2, "gap in pixels between packed sprites to prevent colour bleed (default: 2)")
	flag.BoolVar(&rawOpts.OutTightPacking, "out-tight-packing", true, "crop transparent margins before packing (default: true)")
	flag.BoolVar(&rawOpts.OutPremultipliedAlpha, "out-premultiplied-alpha", false, "premultiply alpha in output texture (default: false)")
	flag.Parse()

	if rawOpts.InAtlas == "" || rawOpts.InTex == "" || rawOpts.OutFile == "" || rawOpts.OutTex == "" {
		fmt.Fprintln(os.Stderr, "usage: libgdx-atlas-to-tileset --in-atlas <in.atlas> --in-tex <in.png> -o <out.tileset> --out-tex <out.png> [--out-width N] [--out-height N] [--out-per-tile-spacing N] [--out-tight-packing=false] [--out-premultiplied-alpha]")
		os.Exit(1)
	}

	opts := convertOptions{
		outWidth:           rawOpts.OutWidth,
		outHeight:          rawOpts.OutHeight,
		perTileSpacing:     rawOpts.OutPerTileSpacing,
		tightPacking:       rawOpts.OutTightPacking,
		premultipliedAlpha: rawOpts.OutPremultipliedAlpha,
	}
	if err := convertAtlas(rawOpts.InAtlas, rawOpts.InTex, rawOpts.OutFile, rawOpts.OutTex, opts); err != nil {
		log.Fatalf("convert: %v", err)
	}
}

func convertAtlas(atlasFile, inTexFile, outFile, outTexFile string, opts convertOptions) error {
	f, err := os.Open(atlasFile)
	if err != nil {
		return fmt.Errorf("open atlas: %w", err)
	}
	defer f.Close()

	parsed, err := parseAtlas(bufio.NewScanner(f))
	if err != nil {
		return fmt.Errorf("parse atlas: %w", err)
	}

	if len(parsed.pages) == 0 {
		return fmt.Errorf("atlas file contains no pages: %s", atlasFile)
	}
	if len(parsed.pages) > 1 {
		return fmt.Errorf("atlas file has %d pages; only single-page atlases are supported", len(parsed.pages))
	}

	page := parsed.pages[0]

	imgFile, err := os.Open(inTexFile)
	if err != nil {
		return fmt.Errorf("open texture: %w", err)
	}
	srcImg, decErr := png.Decode(imgFile)
	imgFile.Close()
	if decErr != nil {
		return fmt.Errorf("decode texture %s: %w", inTexFile, decErr)
	}

	outWidth := opts.outWidth
	outHeight := opts.outHeight
	if outWidth <= 0 {
		if page.width > 0 {
			outWidth = page.width
		} else {
			outWidth = srcImg.Bounds().Dx()
		}
	}
	if outHeight <= 0 {
		if page.height > 0 {
			outHeight = page.height
		} else {
			outHeight = srcImg.Bounds().Dy()
		}
	}

	srcNRGBA := toNRGBA(srcImg)

	atlasGen := &TextureAtlasGenerator{}
	regionByName := map[string]atlasRegion{}

	for _, r := range parsed.regions {
		var img *image.NRGBA
		if r.rotate {
			// NOTE: Spine/libGDX stores the sprite rotated 90° CCW in the atlas.
			// Packed dimensions: size[1] wide × size[0] tall (H_orig × W_orig).
			// Apply 90° CW to restore original size[0] × size[1] (W_orig × H_orig).
			stored := extractRegion(srcNRGBA, r.xy[0], r.xy[1], r.size[1], r.size[0])
			img = rotate90CW(stored)
		} else {
			img = extractRegion(srcNRGBA, r.xy[0], r.xy[1], r.size[0], r.size[1])
		}
		atlasGen.Add(r.name, img, opts.tightPacking)
		regionByName[r.name] = r
	}

	if outWidth <= 0 {
		return fmt.Errorf("output texture width must be > 0 but %d", outWidth)
	}
	if outHeight <= 0 {
		return fmt.Errorf("output texture height must be > 0 but %d", outHeight)
	}

	merged := image.NewNRGBA(image.Rect(0, 0, outWidth, outHeight))

	type regionEntry struct {
		key    uint32
		name   string
		region *schemas.TextureRegionT
	}
	var entries []regionEntry

	textureAtlas := schemas.TextureAtlasT{}

	atlasNode, err := atlasGen.Generate(outWidth, outHeight, opts.perTileSpacing)
	if err != nil {
		return fmt.Errorf("atlasGen.Generate() failed: %w", err)
	}

	atlasNode.Traverse(func(node *TexturePackNode) {
		if node.Source == nil {
			return
		}

		r := regionByName[node.Source.Name]
		src := node.Source.Image
		clipped := node.Source.ClipBounds
		offset := node.Rect.Min

		for y := clipped.Min.Y; y < clipped.Max.Y; y++ {
			for x := clipped.Min.X; x < clipped.Max.X; x++ {
				pixel := src.At(x, y)
				_, _, _, a := pixel.RGBA()
				if a == 0 {
					continue
				}
				merged.Set((x-clipped.Min.X)+offset.X, (y-clipped.Min.Y)+offset.Y, pixel)
			}
		}

		// NOTE: Convert Spine/libGDX's bottom-based y-offset to Pomdog's top-based y-offset:
		// yOffset_top = orig.height - offset_y_bottom - sprite.height
		// When tightPacking is true, clipped.Min.X/Y add the extra trim within the extracted sprite.
		xOffset := int16(r.offset[0] + clipped.Min.X)
		yOffset := int16(r.orig[1]-r.offset[1]-r.size[1]) + int16(clipped.Min.Y)

		entries = append(entries, regionEntry{
			key:  stringhash.StringToHash32(node.Source.Name),
			name: node.Source.Name,
			region: &schemas.TextureRegionT{
				SubrectX:      int16(offset.X),
				SubrectY:      int16(offset.Y),
				SubrectWidth:  int16(clipped.Dx()),
				SubrectHeight: int16(clipped.Dy()),
				Width:         int16(r.orig[0]),
				Height:        int16(r.orig[1]),
				XOffset:       xOffset,
				YOffset:       yOffset,
			},
		})
	})

	// NOTE: sort by key for binary search
	sort.SliceStable(entries, func(i, j int) bool {
		return entries[i].key < entries[j].key
	})

	// NOTE: detect hash collisions offline
	for i := 1; i < len(entries); i++ {
		if entries[i].key == entries[i-1].key {
			return fmt.Errorf("hash collision: XXH32 key %#x for %q and %q", entries[i].key, entries[i-1].name, entries[i].name)
		}
	}

	// NOTE: build parallel keys and regions arrays
	for _, e := range entries {
		textureAtlas.Keys = append(textureAtlas.Keys, e.key)
		textureAtlas.Regions = append(textureAtlas.Regions, e.region)
	}

	if opts.premultipliedAlpha {
		// NOTE: multiply alpha
		multiplyAlpha(merged)
		textureAtlas.PremultipliedAlpha = true
	}

	// NOTE: Write output PNG texture.
	{
		if err := os.MkdirAll(filepath.Dir(outTexFile), fs.ModePerm); err != nil {
			return fmt.Errorf("mkdir %s: %w", filepath.Dir(outTexFile), err)
		}
		f, err := os.Create(outTexFile)
		if err != nil {
			return fmt.Errorf("create %s: %w", outTexFile, err)
		}
		defer f.Close()
		if encErr := png.Encode(f, merged); encErr != nil {
			return fmt.Errorf("encode PNG %s: %w", outTexFile, encErr)
		}
	}

	// NOTE: Write FlatBuffers `.tileset`.
	{
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(textureAtlas.Pack(builder))

		if err := os.MkdirAll(filepath.Dir(outFile), fs.ModePerm); err != nil {
			return fmt.Errorf("mkdir %s: %w", filepath.Dir(outFile), err)
		}
		if err := os.WriteFile(outFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("write %s: %w", outFile, err)
		}
	}

	return nil
}

func toNRGBA(img image.Image) *image.NRGBA {
	if nrgba, ok := img.(*image.NRGBA); ok {
		return nrgba
	}
	bounds := img.Bounds()
	out := image.NewNRGBA(bounds)
	for y := bounds.Min.Y; y < bounds.Max.Y; y++ {
		for x := bounds.Min.X; x < bounds.Max.X; x++ {
			out.Set(x, y, img.At(x, y))
		}
	}
	return out
}

func extractRegion(img *image.NRGBA, x, y, w, h int) *image.NRGBA {
	out := image.NewNRGBA(image.Rect(0, 0, w, h))
	for dy := 0; dy < h; dy++ {
		for dx := 0; dx < w; dx++ {
			out.SetNRGBA(dx, dy, img.NRGBAAt(x+dx, y+dy))
		}
	}
	return out
}

// rotate90CW rotates an image 90° clockwise.
//
// Spine/libGDX stores sprites rotated 90° CCW in the atlas when rotate=true.
// Applying 90° CW restores the original orientation.
//
// For input of width W and height H, the output has width H and height W.
// Output pixel at (ox, oy) = input pixel at (oy, H-1-ox).
func rotate90CW(img *image.NRGBA) *image.NRGBA {
	srcW := img.Bounds().Dx()
	srcH := img.Bounds().Dy()
	out := image.NewNRGBA(image.Rect(0, 0, srcH, srcW))
	for sy := 0; sy < srcH; sy++ {
		for sx := 0; sx < srcW; sx++ {
			ox := srcH - 1 - sy
			oy := sx
			out.SetNRGBA(ox, oy, img.NRGBAAt(sx, sy))
		}
	}
	return out
}
