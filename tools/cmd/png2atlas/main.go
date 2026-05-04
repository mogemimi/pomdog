// Copyright mogemimi. Distributed under the MIT license.

package main

import (
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
	depfile "github.com/mogemimi/pomdog/tools/pkg/depfile"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

func main() {
	var options struct {
		OutFile string
		OutTex  string
		DepFile string
	}
	flag.StringVar(&options.OutFile, "o", "", "output .tileset file path (required)")
	flag.StringVar(&options.OutTex, "out-tex", "", "output PNG texture file path (required)")
	flag.StringVar(&options.DepFile, "d", "", "dependency file path (.d)")
	flag.Parse()

	if options.OutFile == "" || options.OutTex == "" {
		fmt.Fprintln(os.Stderr, "usage: png2atlas -o <output.tileset> --out-tex <output.png> [-d <output.d>] <recipe.toml>...")
		os.Exit(1)
	}

	recipe := &AtlasRecipe{}
	if err := recipe.ReadFiles(flag.Args()); err != nil {
		log.Fatal(err)
	}
	if err := recipe.Validate(); err != nil {
		log.Fatal(err)
	}

	if err := generateAtlas(recipe, options.OutFile, options.OutTex, options.DepFile); err != nil {
		log.Fatal(err)
	}
}

func generateAtlas(recipe *AtlasRecipe, outFile, outTexFile, depFile string) error {
	if recipe.Width <= 0 {
		return fmt.Errorf("recipe.Width must be > 0 but %d", recipe.Width)
	}
	if recipe.Height <= 0 {
		return fmt.Errorf("recipe.Height must be > 0 but %d", recipe.Height)
	}

	atlasGen := &TextureAtlasGenerator{}
	pngDeps := []string{}

	for _, entry := range recipe.Entries {
		pngFile := filepath.Join(entry.SearchDir, entry.Source)

		f, err := os.Open(pngFile)
		if err != nil {
			return fmt.Errorf("open %s: %w", pngFile, err)
		}
		img, err := png.Decode(f)
		f.Close()
		if err != nil {
			return fmt.Errorf("decode %s: %w", pngFile, err)
		}

		atlasGen.Add(entry.Name, img, recipe.TightPacking)
		pngDeps = append(pngDeps, filepath.Clean(pngFile))
	}

	merged := image.NewNRGBA(image.Rect(0, 0, recipe.Width, recipe.Height))
	spacing := recipe.PerTileSpacing
	if spacing < 0 {
		spacing = 0
	}

	type regionEntry struct {
		key    uint32
		name   string
		region *schemas.TextureRegionT
	}
	var entries []regionEntry

	textureAtlas := schemas.TextureAtlasT{}

	atlasNode, err := atlasGen.Generate(recipe.Width, recipe.Height, spacing)
	if err != nil {
		return fmt.Errorf("atlasGen.Generate() failed: %w", err)
	}

	atlasNode.Traverse(func(node *TexturePackNode) {
		if node.Source == nil {
			return
		}

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

		entries = append(entries, regionEntry{
			key:  stringhash.StringToHash32(node.Source.Name),
			name: node.Source.Name,
			region: &schemas.TextureRegionT{
				SubrectX:      int16(offset.X),
				SubrectY:      int16(offset.Y),
				SubrectWidth:  int16(clipped.Dx()),
				SubrectHeight: int16(clipped.Dy()),
				Width:         int16(src.Bounds().Dx()),
				Height:        int16(src.Bounds().Dy()),
				XOffset:       int16(clipped.Min.X),
				YOffset:       int16(clipped.Min.Y),
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

	if recipe.PremultipliedAlpha {
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
		if err := png.Encode(f, merged); err != nil {
			return fmt.Errorf("encode PNG %s: %w", outTexFile, err)
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

	// NOTE: Write dependency file.
	if depFile != "" {
		sort.Strings(pngDeps)
		dep := depfile.DepFile{
			OutFile:  outFile,
			DepFiles: pngDeps,
		}

		if err := os.MkdirAll(filepath.Dir(depFile), fs.ModePerm); err != nil {
			return fmt.Errorf("mkdir %s: %w", filepath.Dir(depFile), err)
		}
		if err := dep.WriteFile(depFile, os.ModePerm); err != nil {
			return fmt.Errorf("write dep file %s: %w", depFile, err)
		}
	}

	return nil
}
