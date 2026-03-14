// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"runtime"
	"sort"

	ninja "github.com/mogemimi/pomdog/tools/pkg/ninja"
	toml "github.com/pelletier/go-toml/v2"
)

type Env struct {
	InRecipe     string
	InSourceDir  string
	OutNinjaFile string
	OutDir       string
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InRecipe, "recipe", "", "copy recipe file (*.toml)")
	flag.StringVar(&env.InSourceDir, "indir", "", "input source directory")
	flag.StringVar(&env.OutNinjaFile, "outninja", "", "output ninja file (build.ninja)")
	flag.StringVar(&env.OutDir, "outdir", "", "output content directory")
	flag.Parse()

	if env.InRecipe == "" {
		log.Fatal("-recipe is required")
	}
	if env.InSourceDir == "" {
		log.Fatal("-indir is required")
	}
	if env.OutNinjaFile == "" {
		log.Fatal("-outninja is required")
	}
	if env.OutDir == "" {
		log.Fatal("-outdir is required")
	}

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

func run(env *Env) error {
	if abs, err := filepath.Abs(env.InSourceDir); err == nil {
		env.InSourceDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutDir); err == nil {
		env.OutDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutNinjaFile); err == nil {
		env.OutNinjaFile = filepath.Clean(abs)
	}

	recipe := &CopyRecipe{}
	if err := recipe.ReadFile(env.InRecipe); err != nil {
		return fmt.Errorf("recipe.ReadFile() failed: %w", err)
	}

	// Collect all copy pairs (src -> dst)
	type copyPair struct {
		Src string // relative to InSourceDir
		Dst string // relative to OutDir
	}

	var pairs []copyPair

	for _, f := range recipe.Files {
		pairs = append(pairs, copyPair{Src: f.Src, Dst: f.Dst})
	}

	for _, fg := range recipe.FileGroups {
		re, err := regexp.Compile(fg.Regex)
		if err != nil {
			return fmt.Errorf("invalid regex %q in [[file_group]]: %w", fg.Regex, err)
		}

		for _, src := range fg.Src {
			if !re.MatchString(src) {
				return fmt.Errorf("[[file_group]] regex %q does not match src %q", fg.Regex, src)
			}
			dst := re.ReplaceAllString(src, fg.Dst)
			pairs = append(pairs, copyPair{Src: src, Dst: dst})
		}
	}

	// Sort for deterministic output
	sort.SliceStable(pairs, func(i, j int) bool {
		return pairs[i].Dst < pairs[j].Dst
	})

	gen := &ninja.Generator{}

	copyCommand := "cp $in $out"

	// NOTE: Windows' cmd.exe does not support "cp" command, but Git Bash provides "cp" command on Windows.
	// If you want to use "copy" command on Windows, set copyCommand to "cmd /c copy /Y $in $out".
	if false {
		if runtime.GOOS == "windows" {
			copyCommand = "cmd /c copy /Y $in $out"
		}
	}

	gen.AddRule(&ninja.Rule{
		Name:    "copy_file",
		Command: copyCommand,
	})

	// Track output directories to create
	outDirs := map[string]struct{}{}

	for _, pair := range pairs {
		srcPath := filepath.Join(env.InSourceDir, pair.Src)
		dstPath := filepath.Join(env.OutDir, pair.Dst)

		outDirs[filepath.Dir(dstPath)] = struct{}{}

		gen.AddBuild(&ninja.Build{
			Rule:    "copy_file",
			OutFile: dstPath,
			InFiles: []string{srcPath},
		})
	}

	// Create output directories
	dirs := make([]string, 0, len(outDirs))
	for d := range outDirs {
		dirs = append(dirs, d)
	}
	sort.Strings(dirs)

	for _, d := range dirs {
		if err := os.MkdirAll(d, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	// Create directory for ninja file
	if dir := filepath.Dir(env.OutNinjaFile); dir != "" {
		if err := os.MkdirAll(dir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	content := gen.Generate()

	f, err := os.Create(env.OutNinjaFile)
	if err != nil {
		return fmt.Errorf("os.Create() failed: %w", err)
	}
	defer f.Close()

	if _, err := f.WriteString(content); err != nil {
		return fmt.Errorf("f.WriteString() failed: %w", err)
	}

	return nil
}

// CopyRecipe represents the TOML recipe for file copy operations.
type CopyRecipe struct {
	Files      []*CopyFile      `toml:"file"`
	FileGroups []*CopyFileGroup `toml:"file_group"`
}

type CopyFile struct {
	Src string `toml:"src"`
	Dst string `toml:"dst"`
}

type CopyFileGroup struct {
	Regex string   `toml:"regex"`
	Dst   string   `toml:"dst"`
	Src   []string `toml:"src"`
}

func (recipe *CopyRecipe) ReadFile(inFile string) error {
	buf, err := os.ReadFile(inFile)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}
	if err := toml.Unmarshal(buf, &recipe); err != nil {
		return fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	if err := recipe.Validate(); err != nil {
		return fmt.Errorf("recipe.Validate() failed: %w", err)
	}

	return nil
}

func (recipe *CopyRecipe) Validate() error {
	for i, f := range recipe.Files {
		if f.Src == "" {
			return fmt.Errorf("[[file]] #%d: src must not be empty", i)
		}
		if f.Dst == "" {
			return fmt.Errorf("[[file]] #%d: dst must not be empty", i)
		}
	}

	for i, fg := range recipe.FileGroups {
		if fg.Regex == "" {
			return fmt.Errorf("[[file_group]] #%d: regex must not be empty", i)
		}
		if fg.Dst == "" {
			return fmt.Errorf("[[file_group]] #%d: dst must not be empty", i)
		}
		if len(fg.Src) == 0 {
			return fmt.Errorf("[[file_group]] #%d: src must not be empty", i)
		}
		if _, err := regexp.Compile(fg.Regex); err != nil {
			return fmt.Errorf("[[file_group]] #%d: invalid regex %q: %w", i, fg.Regex, err)
		}
	}

	return nil
}
