// Copyright mogemimi. Distributed under the MIT license.

package archives

import (
	"bytes"
	"errors"
	"fmt"
	"os"
	"strings"

	toml "github.com/pelletier/go-toml/v2"
)

// Compression method names accepted in archive recipes and by the archive tools.
const (
	// CompressionNone stores files as-is.
	CompressionNone = "none"

	// CompressionZstd stores each file as one Zstandard frame.
	CompressionZstd = "zstd"
)

// ValidateCompression returns an error unless name is a known compression method.
func ValidateCompression(name string) error {
	switch name {
	case CompressionNone, CompressionZstd:
		return nil
	}
	return fmt.Errorf("unknown compression %q (want %q or %q)", name, CompressionNone, CompressionZstd)
}

// decodeTOMLStrict decodes buf, the content of file, into v.
//
// NOTE: It rejects keys that v does not define, so that a misspelled key, such
// as `compresion`, does not silently leave a setting at its fallback value.
func decodeTOMLStrict(file string, buf []byte, v any) error {
	decoder := toml.NewDecoder(bytes.NewReader(buf))
	decoder.DisallowUnknownFields()
	if err := decoder.Decode(v); err != nil {
		var strictErr *toml.StrictMissingError
		if errors.As(err, &strictErr) {
			return fmt.Errorf("unknown keys in %s:\n%s", file, strictErr.String())
		}
		return fmt.Errorf("decoder.Decode() failed: %s: %w", file, err)
	}
	return nil
}

type ArchiveRecipe struct {
	Group []*ArchiveGroup `toml:"group"`
}

type ArchiveGroup struct {
	TargetPlatforms []string `toml:"target_platforms,omitempty"`

	// Compression is the method for the files in this group. When empty, the
	// files use the method of the archive's strategy in the archive settings.
	Compression string `toml:"compression,omitempty"`

	Paths []string `toml:"paths,multiline"`
}

func (recipe *ArchiveRecipe) ReadFile(file string) error {
	buf, err := os.ReadFile(file)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	if err := decodeTOMLStrict(file, buf, recipe); err != nil {
		return err
	}

	for i, group := range recipe.Group {
		// NOTE: Reject unknown platform names, because a misspelled name would
		// silently leave the group out of every archive.
		for _, platform := range group.TargetPlatforms {
			if err := ValidatePlatform(platform); err != nil {
				return fmt.Errorf("%s: group %d: %w", file, i+1, err)
			}
		}
		if group.Compression != "" {
			if err := ValidateCompression(group.Compression); err != nil {
				return fmt.Errorf("%s: group %d: %w", file, i+1, err)
			}
		}
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
