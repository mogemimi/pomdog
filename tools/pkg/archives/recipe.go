package archives

import (
	"bytes"
	"fmt"
	"os"
	"strings"

	toml "github.com/pelletier/go-toml/v2"
)

type ArchiveRecipe struct {
	Group []*ArchiveGroup `toml:"group"`
}

type ArchiveGroup struct {
	TargetPlatforms []string `toml:"target_platforms,omitempty"`
	Paths           []string `toml:"paths,multiline"`
}

func (recipe *ArchiveRecipe) ReadFile(file string) error {
	buf, err := os.ReadFile(file)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	if err := toml.Unmarshal(buf, recipe); err != nil {
		return fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	return nil
}

func (recipe *ArchiveRecipe) ReadFiles(files []string) error {
	for _, file := range files {
		r := ArchiveRecipe{}
		if err := r.ReadFile(file); err != nil {
			return fmt.Errorf("recipe.ReadFile(): %w", err)
		}
		recipe.Group = append(recipe.Group, r.Group...)
	}
	return nil
}

func (recipe *ArchiveRecipe) Encode() (string, error) {
	var buf bytes.Buffer
	enc := toml.NewEncoder(&buf)
	enc.SetIndentSymbol("    ")
	if err := enc.Encode(recipe); err != nil {
		return "", fmt.Errorf("enc.Encode() failed: %w", err)
	}
	s := buf.String()

	// NOTE: Use a trailing comma in array
	s = strings.ReplaceAll(s, "'\n]", "',\n]")

	// NOTE: Use a double-quotes
	s = strings.ReplaceAll(s, `'`, `"`)

	return s, nil
}
