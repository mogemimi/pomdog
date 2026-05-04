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
	"strings"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	depfile "github.com/mogemimi/pomdog/tools/pkg/depfile"
	piskel "github.com/mogemimi/pomdog/tools/pkg/piskel"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

func main() {
	var options struct {
		OutFile     string
		SingleTile  string
		ResizeScale int
		DepFile     string
	}
	flag.StringVar(&options.OutFile, "o", "", "output file (*.png)")
	flag.StringVar(&options.SingleTile, "single", "", "single tile name")
	flag.IntVar(&options.ResizeScale, "resize", 0, "scale for resizing")
	flag.StringVar(&options.DepFile, "d", "", "dep file (*.d)")
	flag.Parse()

	recipe := &TileSetRecipe{}
	if err := recipe.ReadFiles(flag.Args()); err != nil {
		log.Fatal(err)
	}
	if err := recipe.Validate(); err != nil {
		log.Fatal(err)
	}

	if len(options.SingleTile) > 0 {
		if err := generateSingleTile(recipe, options.ResizeScale, options.SingleTile, options.OutFile, options.DepFile); err != nil {
			log.Fatal(err)
		}
	} else {
		if err := generateAtlas(recipe, options.OutFile, options.DepFile); err != nil {
			log.Fatal(err)
		}
	}
}

func generateAtlas(recipe *TileSetRecipe, outFile, depFile string) error {
	piskelMap := map[string]*piskel.Piskel{}
	tileImages := map[string]image.Image{}
	piskelDeps := []string{}

	for _, tile := range recipe.Tiles {
		key := tile.Source
		piskelFile := filepath.Join(tile.SearchDir, tile.Source)

		if _, ok := piskelMap[key]; ok {
			continue
		}

		p := &piskel.Piskel{}
		if err := p.ReadFile(piskelFile); err != nil {
			return fmt.Errorf("piskel.ReadFile() failed: %w", err)
		}
		piskelMap[key] = p
		piskelDeps = append(piskelDeps, piskelFile)
	}

	genImageKey := func(tile *Tile) string {
		return fmt.Sprintf("%s_%d", tile.Source, tile.Index)
	}

	atlasGen := &TextureAtlasGenerator{}

	for _, tile := range recipe.Tiles {
		p := piskelMap[tile.Source]

		layerMerged, err := renderTile(tile, p)
		if err != nil {
			return fmt.Errorf("renderTile() failed: %w", err)
		}

		tileImages[genImageKey(tile)] = layerMerged
		atlasGen.Add(tile.Name, layerMerged, recipe.TightPacking)
	}

	if recipe.Width <= 0 {
		return fmt.Errorf("recipe.Width must be > 0 but %d", recipe.Width)
	}
	if recipe.Height <= 0 {
		return fmt.Errorf("recipe.Height must be > 0 but %d", recipe.Height)
	}
	if recipe.PerTileSpacing < 0 {
		return fmt.Errorf("recipe.PerTileSpacing must be >= 0 but %d", recipe.PerTileSpacing)
	}
	if recipe.TileBorderSize < 0 {
		return fmt.Errorf("recipe.TileBorderSize must be >= 0 but %d", recipe.TileBorderSize)
	}

	merged := image.NewNRGBA(image.Rect(0, 0, recipe.Width, recipe.Height))

	textureAtlas := schemas.TextureAtlasT{}

	atlasNode, err := atlasGen.Generate(recipe.Width, recipe.Height, recipe.PerTileSpacing+recipe.TileBorderSize*2)
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

		if clipped.Dx() > 0 && clipped.Dy() > 0 && recipe.TileBorderSize > 0 {
			offset.X += recipe.TileBorderSize
			offset.Y += recipe.TileBorderSize

			// NOTE: Render edge-border for pseudo-clamped texture sampling.
			for i := 0; i < recipe.TileBorderSize; i++ {
				borderX := offset.X - 1 - i
				x := clipped.Min.X
				for y := clipped.Min.Y; y < clipped.Max.Y; y++ {
					pixel := src.At(x, y)
					if _, _, _, a := pixel.RGBA(); a > 0 {
						merged.Set((x-clipped.Min.X)+borderX, (y-clipped.Min.Y)+offset.Y, pixel)
					}
				}
			}
			for i := 0; i < recipe.TileBorderSize; i++ {
				borderX := offset.X + 1 + i
				x := clipped.Max.X - 1
				for y := clipped.Min.Y; y < clipped.Max.Y; y++ {
					pixel := src.At(x, y)
					if _, _, _, a := pixel.RGBA(); a > 0 {
						merged.Set((x-clipped.Min.X)+borderX, (y-clipped.Min.Y)+offset.Y, pixel)
					}
				}
			}
			for i := 0; i < recipe.TileBorderSize; i++ {
				borderY := offset.Y - 1 - i
				y := clipped.Min.Y
				for x := clipped.Min.X; x < clipped.Max.X; x++ {
					pixel := src.At(x, y)
					if _, _, _, a := pixel.RGBA(); a > 0 {
						merged.Set((x-clipped.Min.X)+offset.X, (y-clipped.Min.Y)+borderY, pixel)
					}
				}
			}
			for i := 0; i < recipe.TileBorderSize; i++ {
				borderY := offset.Y + 1 + i
				y := clipped.Max.Y - 1
				for x := clipped.Min.X; x < clipped.Max.X; x++ {
					pixel := src.At(x, y)
					if _, _, _, a := pixel.RGBA(); a > 0 {
						merged.Set((x-clipped.Min.X)+offset.X, (y-clipped.Min.Y)+borderY, pixel)
					}
				}
			}
		}

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

		textureAtlas.Regions = append(textureAtlas.Regions, &schemas.TextureRegionT{
			Key:           stringhash.StringToHash32(node.Source.Name),
			SubrectX:      int16(offset.X),
			SubrectY:      int16(offset.Y),
			SubrectWidth:  int16(clipped.Dx()),
			SubrectHeight: int16(clipped.Dy()),
			Width:         int16(src.Bounds().Dx()),
			Height:        int16(src.Bounds().Dy()),
			XOffset:       int16(clipped.Min.X),
			YOffset:       int16(clipped.Min.Y),
		})
	})

	// NOTE: sort by key for binary-search
	sort.SliceStable(textureAtlas.Regions, func(i, j int) bool {
		return textureAtlas.Regions[i].Key < textureAtlas.Regions[j].Key
	})

	if recipe.PremultipliedAlpha {
		// NOTE: multiply alpha
		multiplyAlpha(merged)
		textureAtlas.PremultipliedAlpha = true
	}

	{
		f, err := os.Create(outFile)
		if err != nil {
			return fmt.Errorf("os.Create() failed: %w", err)
		}
		defer f.Close()

		if err := png.Encode(f, merged); err != nil {
			return fmt.Errorf("png.Encode() failed: %w", err)
		}
	}
	{
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(textureAtlas.Pack(builder))

		file := strings.TrimSuffix(outFile, ".png") + ".tileset"

		if err := os.WriteFile(file, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	if len(depFile) > 0 {
		dep := depfile.DepFile{
			OutFile: outFile,
		}

		for i, f := range piskelDeps {
			f = filepath.Clean(f)
			piskelDeps[i] = f
		}
		sort.SliceStable(piskelDeps, func(i, j int) bool {
			return piskelDeps[i] < piskelDeps[j]
		})
		dep.DepFiles = append(dep.DepFiles, piskelDeps...)

		if err := dep.WriteFile(depFile, os.ModePerm); err != nil {
			return fmt.Errorf("dep.WriteFile() failed: %w", err)
		}
	}

	return nil
}

// generateSingleTile renders a single named tile from the recipe and writes it as a
// standalone PNG image. Unlike generateAtlas, this function does not pack multiple tiles
// together and does not output a .tileset binary - it is intended for exporting individual
// sprites (e.g. app icons) at a specified scale.
func generateSingleTile(recipe *TileSetRecipe, resizeScale int, tileName, outFile, depFile string) error {
	piskelDeps := []string{}

	tile := func() *Tile {
		for _, tile := range recipe.Tiles {
			if tile.Name == tileName {
				return tile
			}
		}
		return nil
	}()

	if tile == nil {
		return fmt.Errorf("cannot find %s", tileName)
	}

	piskelFile := filepath.Join(tile.SearchDir, tile.Source)

	p := &piskel.Piskel{}
	if err := p.ReadFile(piskelFile); err != nil {
		return fmt.Errorf("piskel.ReadFile() failed: %w", err)
	}
	piskelDeps = append(piskelDeps, piskelFile)

	merged, err := renderTile(tile, p)
	if err != nil {
		return fmt.Errorf("renderTile() failed: %w", err)
	}

	if recipe.PremultipliedAlpha {
		// NOTE: multiply alpha
		multiplyAlpha(merged)
	}

	if resizeScale >= 2 {
		// NOTE: resize image
		if resized, err := resizeImage(merged, resizeScale); err != nil {
			return fmt.Errorf("resizeImage() failed: %w", err)
		} else {
			merged = resized
		}
	}

	{
		f, err := os.Create(outFile)
		if err != nil {
			return fmt.Errorf("os.Create() failed: %w", err)
		}
		defer f.Close()

		if err := png.Encode(f, merged); err != nil {
			return fmt.Errorf("png.Encode() failed: %w", err)
		}
	}

	if len(depFile) > 0 {
		dep := depfile.DepFile{
			OutFile: outFile,
		}

		for i, f := range piskelDeps {
			f = filepath.Clean(f)
			piskelDeps[i] = f
		}
		sort.SliceStable(piskelDeps, func(i, j int) bool {
			return piskelDeps[i] < piskelDeps[j]
		})
		dep.DepFiles = append(dep.DepFiles, piskelDeps...)

		if err := dep.WriteFile(depFile, os.ModePerm); err != nil {
			return fmt.Errorf("dep.WriteFile() failed: %w", err)
		}
	}

	return nil
}
