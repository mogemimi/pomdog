// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	toml "github.com/pelletier/go-toml/v2"
)

// Recipe is the internal representation after parsing shaderbuild.toml.
type Recipe struct {
	Builds []*RecipeBuild
	Links  []*RecipeLink
}

type RecipeBuild struct {
	Source     string
	Name       string
	Stage      string // "vs", "ps", "cs"
	EntryPoint string
	VSOut      string // PS only: name of VS build whose output interface to inherit
}

type RecipeLink struct {
	Name string
	VS   string
	PS   string
}

// recipeFile is the on-disk TOML representation.
type recipeFile struct {
	CompileVS []*recipeCompileVS `toml:"compile_vs"`
	CompilePS []*recipeCompilePS `toml:"compile_ps"`
	Links     []*recipeFileLink  `toml:"link"`
}

type recipeCompileVS struct {
	Source     string `toml:"source"`
	Name       string `toml:"name"`
	EntryPoint string `toml:"entrypoint"`
}

type recipeCompilePS struct {
	Source     string `toml:"source"`
	Name       string `toml:"name"`
	EntryPoint string `toml:"entrypoint"`
	VSOut      string `toml:"vsout"`
}

type recipeFileLink struct {
	Name string `toml:"name"`
	VS   string `toml:"vs"`
	PS   string `toml:"ps"`
}

func (recipe *Recipe) ReadFile(inFile string) error {
	buf, err := os.ReadFile(inFile)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	var rf recipeFile
	if err := toml.Unmarshal(buf, &rf); err != nil {
		return fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	// Convert to internal representation.
	for _, vs := range rf.CompileVS {
		b := &RecipeBuild{
			Source:     vs.Source,
			Name:       vs.Name,
			Stage:      "vs",
			EntryPoint: vs.EntryPoint,
		}
		recipe.Builds = append(recipe.Builds, b)
	}
	for _, ps := range rf.CompilePS {
		b := &RecipeBuild{
			Source:     ps.Source,
			Name:       ps.Name,
			Stage:      "ps",
			EntryPoint: ps.EntryPoint,
			VSOut:      ps.VSOut,
		}
		recipe.Builds = append(recipe.Builds, b)
	}
	for _, link := range rf.Links {
		recipe.Links = append(recipe.Links, &RecipeLink{
			Name: link.Name,
			VS:   link.VS,
			PS:   link.PS,
		})
	}

	if err := recipe.Complement(); err != nil {
		return fmt.Errorf("recipe.Complement() failed: %w", err)
	}
	if err := recipe.Validate(); err != nil {
		return fmt.Errorf("recipe.Validate() failed: %w", err)
	}

	return nil
}

func (recipe *Recipe) Complement() error {
	for _, build := range recipe.Builds {
		if len(build.Source) == 0 {
			return fmt.Errorf("len(build.Source) must be > 0")
		}

		if len(build.Name) == 0 {
			ext := filepath.Ext(build.Source)
			build.Name = strings.TrimSuffix(build.Source, ext)
		}

		if len(build.EntryPoint) == 0 {
			build.EntryPoint = "main"
		}
	}

	return nil
}

func (recipe *Recipe) Validate() error {
	for _, build := range recipe.Builds {
		if len(build.Source) == 0 {
			return fmt.Errorf("len(build.Source) must be > 0")
		}

		if len(build.Name) == 0 {
			return fmt.Errorf("len(build.Name) must be > 0")
		}

		if len(build.EntryPoint) == 0 {
			return fmt.Errorf("len(build.EntryPoint) must be > 0")
		}

		switch build.Stage {
		case "vs", "ps", "cs":
			// NOTE: valid
		default:
			return fmt.Errorf("unknown shader stage %q for build %q", build.Stage, build.Name)
		}
	}

	// Build a name set for cross-referencing
	buildNames := make(map[string]string) // name -> stage
	for _, build := range recipe.Builds {
		buildNames[build.Name] = build.Stage
	}

	// Validate vsout references
	for _, build := range recipe.Builds {
		if build.VSOut != "" {
			if stage, ok := buildNames[build.VSOut]; !ok {
				return fmt.Errorf("[[compile_ps]] %q: vsout %q not found in [[compile_vs]]", build.Name, build.VSOut)
			} else if stage != "vs" {
				return fmt.Errorf("[[compile_ps]] %q: vsout %q has stage %q, expected vs", build.Name, build.VSOut, stage)
			}
		}
	}

	// Validate link references
	for _, link := range recipe.Links {
		if len(link.Name) == 0 {
			return fmt.Errorf("[[link]]: name must not be empty")
		}
		if len(link.VS) == 0 {
			return fmt.Errorf("[[link]] %q: vs must not be empty", link.Name)
		}
		if len(link.PS) == 0 {
			return fmt.Errorf("[[link]] %q: ps must not be empty", link.Name)
		}

		if stage, ok := buildNames[link.VS]; !ok {
			return fmt.Errorf("[[link]] %q: vs %q not found in [[compile_vs]]", link.Name, link.VS)
		} else if stage != "vs" {
			return fmt.Errorf("[[link]] %q: vs %q has stage %q, expected vs", link.Name, link.VS, stage)
		}

		if stage, ok := buildNames[link.PS]; !ok {
			return fmt.Errorf("[[link]] %q: ps %q not found in [[compile_ps]]", link.Name, link.PS)
		} else if stage != "ps" {
			return fmt.Errorf("[[link]] %q: ps %q has stage %q, expected ps", link.Name, link.PS, stage)
		}
	}

	return nil
}
