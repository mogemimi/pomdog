// Copyright mogemimi. Distributed under the MIT license.

package archives

import (
	"errors"
	"fmt"
	"os"
	"slices"
)

// Platform names used by archive recipes, archive settings, and archive-content.
// Each name is also the subdirectory of the platform's shipping archive.
const (
	PlatformWindows = "windows"
	PlatformMacOS   = "macos"
	PlatformLinux   = "linux"
	PlatformWeb     = "web"
)

// ValidatePlatform returns an error unless name is a known platform.
func ValidatePlatform(name string) error {
	switch name {
	case PlatformWindows, PlatformMacOS, PlatformLinux, PlatformWeb:
		return nil
	}
	return fmt.Errorf("unknown platform %q (want %q, %q, %q, or %q)", name, PlatformWindows, PlatformMacOS, PlatformLinux, PlatformWeb)
}

// ArchiveSettings describes how archive-content stores files in the archive
// of each platform.
type ArchiveSettings struct {
	Strategy []*ArchiveStrategy `toml:"strategy"`
}

// ArchiveStrategy describes how the archives of its target platforms store files.
type ArchiveStrategy struct {
	// TargetPlatforms lists the platforms whose archives use this strategy.
	// A platform belongs to at most one strategy.
	TargetPlatforms []string `toml:"target_platforms"`

	// Compression is the method for recipe groups that omit `compression`.
	Compression string `toml:"compression"`

	// Zstd decides whether a file is worth storing as a zstd frame. It is
	// required when this strategy or a recipe group selects zstd.
	Zstd *ZstdSettings `toml:"zstd"`
}

// ZstdSettings describes how to compress files with zstd and when to keep the
// frames. A file is stored as-is unless its frame saves at least MinSavedBytes
// and is at most MaxRatioPercent of the file size, so the saved bytes justify
// the decoding cost.
type ZstdSettings struct {
	// NOTE: Pointers distinguish a missing key from zero, so every setting is explicit.

	// Level is the zstd compression level, from 1 to 22.
	Level           *int     `toml:"level"`
	MinSavedBytes   *int64   `toml:"min_saved_bytes"`
	MaxRatioPercent *float64 `toml:"max_ratio_percent"`

	// Dictionary lists the dictionaries that zstd uses for the files that match
	// their paths. A file matches at most one dictionary.
	Dictionary []*ZstdDictionarySettings `toml:"dictionary"`
}

// ZstdDictionarySettings describes a zstd dictionary that the archive tools
// train on the files that use it.
type ZstdDictionarySettings struct {
	// Name identifies the dictionary in the archive tools and in the debug index.
	Name string `toml:"name"`

	// MaxSize is the maximum size of the dictionary in bytes.
	MaxSize *int `toml:"max_size"`

	// Paths lists glob patterns of the files that use this dictionary, as listed
	// in the archive recipes. See MatchPath for the pattern syntax.
	Paths []string `toml:"paths"`
}

const (
	// ZstdMinLevel and ZstdMaxLevel bound the compression levels in archive settings.
	ZstdMinLevel = 1
	ZstdMaxLevel = 22

	// ZstdMinDictionarySize is the smallest dictionary that zstd can train.
	ZstdMinDictionarySize = 256

	// ZstdMaxDictionaries is the number of dictionaries that an archive entry
	// can refer to with its 8-bit dictionary index.
	ZstdMaxDictionaries = 256
)

// ReadFile reads the archive settings in file and validates them.
func (settings *ArchiveSettings) ReadFile(file string) error {
	buf, err := os.ReadFile(file)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	if err := decodeTOMLStrict(file, buf, settings); err != nil {
		return err
	}

	if err := settings.validate(); err != nil {
		return fmt.Errorf("%s: %w", file, err)
	}
	return nil
}

func (settings *ArchiveSettings) validate() error {
	if len(settings.Strategy) == 0 {
		return errors.New("no [[strategy]] tables")
	}

	// NOTE: Map each platform to the strategy that targets it to detect duplicates.
	platformStrategies := map[string]int{}

	for i, strategy := range settings.Strategy {
		if len(strategy.TargetPlatforms) == 0 {
			return fmt.Errorf("strategy %d: target_platforms is empty", i+1)
		}
		for _, platform := range strategy.TargetPlatforms {
			if err := ValidatePlatform(platform); err != nil {
				return fmt.Errorf("strategy %d: %w", i+1, err)
			}
			if j, ok := platformStrategies[platform]; ok {
				return fmt.Errorf("strategies %d and %d both target %q", j+1, i+1, platform)
			}
			platformStrategies[platform] = i
		}

		if strategy.Compression == "" {
			return fmt.Errorf("strategy %d: compression is required", i+1)
		}
		if err := ValidateCompression(strategy.Compression); err != nil {
			return fmt.Errorf("strategy %d: %w", i+1, err)
		}
		if strategy.Compression == CompressionZstd && strategy.Zstd == nil {
			return fmt.Errorf("strategy %d: compression is %q, but [strategy.zstd] is missing", i+1, CompressionZstd)
		}
		if strategy.Zstd != nil {
			if err := strategy.Zstd.validate(); err != nil {
				return fmt.Errorf("strategy %d: zstd: %w", i+1, err)
			}
		}
	}

	return nil
}

func (s *ZstdSettings) validate() error {
	if s.Level == nil {
		return errors.New("level is required")
	}
	if *s.Level < ZstdMinLevel || *s.Level > ZstdMaxLevel {
		return fmt.Errorf("level must be from %d to %d, got %d", ZstdMinLevel, ZstdMaxLevel, *s.Level)
	}
	if s.MinSavedBytes == nil {
		return errors.New("min_saved_bytes is required")
	}
	if *s.MinSavedBytes < 1 {
		return fmt.Errorf("min_saved_bytes must be at least 1, got %d", *s.MinSavedBytes)
	}
	if s.MaxRatioPercent == nil {
		return errors.New("max_ratio_percent is required")
	}
	if !(*s.MaxRatioPercent > 0 && *s.MaxRatioPercent <= 100) {
		return fmt.Errorf("max_ratio_percent must be greater than 0 and at most 100, got %v", *s.MaxRatioPercent)
	}

	if len(s.Dictionary) > ZstdMaxDictionaries {
		return fmt.Errorf("%d dictionaries, but a strategy can have at most %d", len(s.Dictionary), ZstdMaxDictionaries)
	}

	names := map[string]struct{}{}
	for i, dictionary := range s.Dictionary {
		if err := dictionary.validate(); err != nil {
			return fmt.Errorf("dictionary %d: %w", i+1, err)
		}
		if _, ok := names[dictionary.Name]; ok {
			return fmt.Errorf("dictionary %d: duplicate name %q", i+1, dictionary.Name)
		}
		names[dictionary.Name] = struct{}{}
	}
	return nil
}

func (d *ZstdDictionarySettings) validate() error {
	if d.Name == "" {
		return errors.New("name is required")
	}
	// NOTE: The name appears in the debug index and in the output of the archive
	// tools, so it is limited to characters that need no quoting.
	for _, c := range d.Name {
		if !(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_' || c == '-') {
			return fmt.Errorf("name %q contains %q; use letters, digits, '_', and '-'", d.Name, c)
		}
	}
	if d.MaxSize == nil {
		return fmt.Errorf("%s: max_size is required", d.Name)
	}
	if *d.MaxSize < ZstdMinDictionarySize {
		return fmt.Errorf("%s: max_size must be at least %d, got %d", d.Name, ZstdMinDictionarySize, *d.MaxSize)
	}
	if len(d.Paths) == 0 {
		return fmt.Errorf("%s: paths is empty", d.Name)
	}
	for _, pattern := range d.Paths {
		if err := ValidatePathPattern(pattern); err != nil {
			return fmt.Errorf("%s: %w", d.Name, err)
		}
	}
	return nil
}

// StrategyFor returns the strategy whose target platforms include platform.
func (settings *ArchiveSettings) StrategyFor(platform string) (*ArchiveStrategy, error) {
	if err := ValidatePlatform(platform); err != nil {
		return nil, err
	}
	for _, strategy := range settings.Strategy {
		if slices.Contains(strategy.TargetPlatforms, platform) {
			return strategy, nil
		}
	}
	return nil, fmt.Errorf("no strategy targets %q", platform)
}

// Accepts reports whether a zstd frame of compressedSize bytes is worth storing
// in place of the uncompressedSize bytes of the original file.
func (s *ZstdSettings) Accepts(uncompressedSize, compressedSize int) bool {
	saved := int64(uncompressedSize) - int64(compressedSize)
	if saved < *s.MinSavedBytes {
		return false
	}

	// NOTE: MinSavedBytes is at least 1, so uncompressedSize is not zero here.
	ratioPercent := float64(compressedSize) * 100 / float64(uncompressedSize)
	return ratioPercent <= *s.MaxRatioPercent
}
