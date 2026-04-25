// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"os"
	"path/filepath"
	"sort"

	"github.com/pelletier/go-toml/v2"
)

type TileSetRecipe struct {
	Tiles              []*Tile `toml:"tiles"`
	Width              int     `toml:"width"`
	Height             int     `toml:"height"`
	PerTileSpacing     int     `toml:"per_tile_spacing"`
	TileBorderSize     int     `toml:"tile_border_size"`
	TightPacking       bool    `toml:"tight_packing"`
	PremultipliedAlpha bool    `toml:"premultiplied_alpha"`
}

type Tile struct {
	Name         string    `toml:"name"`
	Source       string    `toml:"source"`
	SearchDir    string    `toml:"-"`
	Index        int       `toml:"index"`
	Layers       []int     `toml:"layers"`
	Comment      string    `toml:"comment"`
	Opacity      int       `toml:"opacity"`
	OpacityLayer *int      `toml:"opacity_layer"`
	MaskRect     *TileRect `toml:"mask_rect"`
}

type TileRect struct {
	MinX int `toml:"min_x"`
	MinY int `toml:"min_y"`
	MaxX int `toml:"max_x"`
	MaxY int `toml:"max_y"`
}

func (recipe *TileSetRecipe) ReadFiles(inFiles []string) error {
	for _, file := range inFiles {
		re := &TileSetRecipe{}

		buf, err := os.ReadFile(file)
		if err != nil {
			return fmt.Errorf("os.ReadFile() failed: %w", err)
		}

		if err := toml.Unmarshal(buf, re); err != nil {
			return fmt.Errorf("toml.Unmarshal() failed: %w", err)
		}

		dir := filepath.Clean(filepath.Dir(file))
		for _, tile := range re.Tiles {
			tile.SearchDir = dir
		}

		// NOTE: Merge tiles from multiple recipe files.
		recipe.Tiles = append(recipe.Tiles, re.Tiles...)
		if recipe.Width < re.Width {
			recipe.Width = re.Width
		}
		if recipe.Height < re.Height {
			recipe.Height = re.Height
		}
		if recipe.PerTileSpacing < re.PerTileSpacing {
			recipe.PerTileSpacing = re.PerTileSpacing
		}
		if recipe.TileBorderSize < re.TileBorderSize {
			recipe.TileBorderSize = re.TileBorderSize
		}
		if re.TightPacking {
			recipe.TightPacking = true
		}
		if re.PremultipliedAlpha {
			recipe.PremultipliedAlpha = true
		}
	}

	sort.SliceStable(recipe.Tiles, func(i, j int) bool {
		return recipe.Tiles[i].Name < recipe.Tiles[j].Name
	})

	return nil
}

func (recipe *TileSetRecipe) Validate() error {
	tiles := map[string]struct{}{}
	for _, tile := range recipe.Tiles {
		if len(tile.Name) == 0 {
			return fmt.Errorf("empty name")
		}
		if _, ok := tiles[tile.Name]; ok {
			return fmt.Errorf("duplicate name '%s'", tile.Name)
		}
		if len(tile.Source) == 0 {
			return fmt.Errorf("empty source at '%s'", tile.Name)
		}
		tiles[tile.Name] = struct{}{}
	}

	return nil
}

func (tile *Tile) LayerEnabled(layerIndex int) bool {
	if len(tile.Layers) == 0 {
		return true
	}
	for _, layer := range tile.Layers {
		if layer == layerIndex {
			return true
		}
	}
	return false
}
