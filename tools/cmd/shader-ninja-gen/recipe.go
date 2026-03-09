// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	toml "github.com/pelletier/go-toml/v2"
)

type Recipe struct {
	Builds []*RecipeBuild `toml:"builds"`
	Links  []*RecipeLink  `toml:"link"`
}

type RecipeBuild struct {
	Source     string `toml:"source"`
	Name       string `toml:"name"`
	Stage      string `toml:"stage"`
	EntryPoint string `toml:"entrypoint"`
}

type RecipeLink struct {
	Name string `toml:"name"`
	VS   string `toml:"vs"`
	PS   string `toml:"ps"`
}

func (recipe *Recipe) ReadFile(inFile string) error {
	buf, err := os.ReadFile(inFile)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}
	if err := toml.Unmarshal(buf, &recipe); err != nil {
		return fmt.Errorf("toml.Unmarshal() failed: %w", err)
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
		if len(build.Stage) == 0 {
			name := build.Name

			if strings.HasSuffix(name, "_vs") {
				build.Stage = "vs"
			} else if strings.HasSuffix(name, "_ps") {
				build.Stage = "ps"
			} else if strings.HasSuffix(name, "_cs") {
				build.Stage = "cs"
			} else {
				return fmt.Errorf("unknown shader stage")
			}
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
			return fmt.Errorf("unknown shader stage")
		}
	}

	// Build a name set for cross-referencing link entries
	buildNames := make(map[string]string) // name -> stage
	for _, build := range recipe.Builds {
		buildNames[build.Name] = build.Stage
	}

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
			return fmt.Errorf("[[link]] %q: vs %q not found in [[builds]]", link.Name, link.VS)
		} else if stage != "vs" {
			return fmt.Errorf("[[link]] %q: vs %q has stage %q, expected vs", link.Name, link.VS, stage)
		}

		if stage, ok := buildNames[link.PS]; !ok {
			return fmt.Errorf("[[link]] %q: ps %q not found in [[builds]]", link.Name, link.PS)
		} else if stage != "ps" {
			return fmt.Errorf("[[link]] %q: ps %q has stage %q, expected ps", link.Name, link.PS, stage)
		}
	}

	return nil
}
