// Copyright mogemimi. Distributed under the MIT license.

package archives

import (
	"fmt"
	"path/filepath"
	"slices"
	"sort"
	"strconv"
	"strings"
)

// ArchivePlan lists the files that one archive stores and the method that the
// archive settings and recipes request for each file.
type ArchivePlan struct {
	// Strategy is the strategy of the target platforms of the archive.
	Strategy *ArchiveStrategy

	// Files lists the files sorted by path.
	Files []*PlannedFile

	// Dictionaries lists the zstd dictionaries that at least one file uses, in
	// the order of the archive settings.
	Dictionaries []*PlannedDictionary
}

// PlannedFile is a file that an archive stores.
type PlannedFile struct {
	// Path is the path as listed in the archive recipes.
	Path string

	// Compression is the method that the recipe group or the strategy requests.
	Compression string

	// Dictionary is the zstd dictionary that compresses the file, or nil.
	Dictionary *PlannedDictionary
}

// PlannedDictionary is a zstd dictionary trained on the files that use it.
type PlannedDictionary struct {
	Settings *ZstdDictionarySettings

	// ID is the dictionary ID that the dictionary and its frames record. It is
	// the position of the dictionary in the strategy plus one, so it is unique
	// in the archive and does not depend on the files.
	ID uint32

	// Files lists the paths of the files that use the dictionary, sorted by path.
	Files []string
}

// PlanArchive returns the plan of the archive for platforms. All platforms must
// share one strategy, because an archive stores its files in one way.
func PlanArchive(settings *ArchiveSettings, recipe *ArchiveRecipe, platforms []string) (*ArchivePlan, error) {
	if len(platforms) == 0 {
		return nil, fmt.Errorf("no target platforms")
	}

	var strategy *ArchiveStrategy
	for _, platform := range platforms {
		s, err := settings.StrategyFor(platform)
		if err != nil {
			return nil, err
		}
		if strategy != nil && s != strategy {
			return nil, fmt.Errorf("platforms %s use different strategies", strings.Join(platforms, ", "))
		}
		strategy = s
	}

	plan := &ArchivePlan{Strategy: strategy}

	addedFiles := map[string]struct{}{}
	for _, group := range recipe.Group {
		if len(group.TargetPlatforms) > 0 && !slices.ContainsFunc(group.TargetPlatforms, func(p string) bool {
			return slices.Contains(platforms, p)
		}) {
			continue
		}

		compression := strategy.Compression
		if group.Compression != "" {
			compression = group.Compression
		}
		if compression == CompressionZstd && strategy.Zstd == nil && len(group.Paths) > 0 {
			return nil, fmt.Errorf("the strategy for %s has no [strategy.zstd] settings for the recipe group with %q", strings.Join(platforms, ", "), group.Paths[0])
		}

		for _, path := range group.Paths {
			if _, ok := addedFiles[path]; ok {
				return nil, fmt.Errorf("already exists: %v", path)
			}
			addedFiles[path] = struct{}{}
			plan.Files = append(plan.Files, &PlannedFile{Path: path, Compression: compression})
		}
	}
	sort.SliceStable(plan.Files, func(i, j int) bool {
		return plan.Files[i].Path < plan.Files[j].Path
	})

	if strategy.Zstd == nil {
		return plan, nil
	}

	dictionaries := make([]*PlannedDictionary, len(strategy.Zstd.Dictionary))
	for i, d := range strategy.Zstd.Dictionary {
		dictionaries[i] = &PlannedDictionary{Settings: d, ID: uint32(i + 1)}
	}

	for _, file := range plan.Files {
		if file.Compression != CompressionZstd {
			continue
		}
		for _, d := range dictionaries {
			if !slices.ContainsFunc(d.Settings.Paths, func(pattern string) bool {
				return MatchPath(pattern, file.Path)
			}) {
				continue
			}
			if file.Dictionary != nil {
				return nil, fmt.Errorf("%s matches both dictionaries %q and %q", file.Path, file.Dictionary.Settings.Name, d.Settings.Name)
			}
			file.Dictionary = d
			d.Files = append(d.Files, file.Path)
		}
	}

	for _, d := range dictionaries {
		if len(d.Files) > 0 {
			plan.Dictionaries = append(plan.Dictionaries, d)
		}
	}

	return plan, nil
}

// ZstdFramePath returns the path under dir of the zstd frame of file, a path as
// listed in the archive recipes.
func ZstdFramePath(dir, file string) string {
	return filepath.Join(dir, "files", filepath.FromSlash(file)+".zst")
}

// ZstdDictionaryPath returns the path under dir of the zstd dictionary with the
// dictionary ID id.
//
// NOTE: The path uses the ID instead of the name, because names that differ only
// in case, or names reserved by the operating system, such as `con` on Windows,
// cannot be file names on every file system.
func ZstdDictionaryPath(dir string, id uint32) string {
	return filepath.Join(dir, "dictionaries", strconv.FormatUint(uint64(id), 10)+".dict")
}
