// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"os"
	"path/filepath"
	"sort"

	"github.com/pelletier/go-toml/v2"
)

// AtlasRecipe describes all settings for building a texture atlas from PNG files.
type AtlasRecipe struct {
	Entries            []*AtlasEntry `toml:"entries"`
	Width              int           `toml:"width"`
	Height             int           `toml:"height"`
	PerTileSpacing     int           `toml:"per_tile_spacing"`
	TightPacking       bool          `toml:"tight_packing"`
	PremultipliedAlpha bool          `toml:"premultiplied_alpha"`
}

// AtlasEntry describes one PNG file to include in the atlas.
type AtlasEntry struct {
	Name      string `toml:"name"`
	Source    string `toml:"source"`
	SearchDir string `toml:"-"`
}

func (recipe *AtlasRecipe) ReadFiles(inFiles []string) error {
	for _, file := range inFiles {
		re := &AtlasRecipe{}

		buf, err := os.ReadFile(file)
		if err != nil {
			return fmt.Errorf("os.ReadFile(%s): %w", file, err)
		}
		if err := toml.Unmarshal(buf, re); err != nil {
			return fmt.Errorf("toml.Unmarshal(%s): %w", file, err)
		}

		dir := filepath.Clean(filepath.Dir(file))
		for _, e := range re.Entries {
			e.SearchDir = dir
		}

		recipe.Entries = append(recipe.Entries, re.Entries...)

		if recipe.Width < re.Width {
			recipe.Width = re.Width
		}
		if recipe.Height < re.Height {
			recipe.Height = re.Height
		}
		if recipe.PerTileSpacing < re.PerTileSpacing {
			recipe.PerTileSpacing = re.PerTileSpacing
		}
		if re.TightPacking {
			recipe.TightPacking = true
		}
		if re.PremultipliedAlpha {
			recipe.PremultipliedAlpha = true
		}
	}

	sort.SliceStable(recipe.Entries, func(i, j int) bool {
		return recipe.Entries[i].Name < recipe.Entries[j].Name
	})

	return nil
}

func (recipe *AtlasRecipe) Validate() error {
	seen := map[string]struct{}{}
	for _, e := range recipe.Entries {
		if e.Name == "" {
			return fmt.Errorf("empty name in entry")
		}
		if _, ok := seen[e.Name]; ok {
			return fmt.Errorf("duplicate name %q", e.Name)
		}
		if e.Source == "" {
			return fmt.Errorf("empty source for entry %q", e.Name)
		}
		seen[e.Name] = struct{}{}
	}
	return nil
}
