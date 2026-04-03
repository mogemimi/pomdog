// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strings"

	ninja "github.com/mogemimi/pomdog/tools/pkg/ninja"
	toml "github.com/pelletier/go-toml/v2"
)

type Env struct {
	InRecipe      string
	InDir         string
	ThirdPartyDir string
	OutNinjaFile  string
	OutDir        string
	IntDir        string
	ToolDir       string
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InRecipe, "recipe", "", "convert recipe file (*.toml)")
	flag.StringVar(&env.InDir, "indir", "", "input asset directory")
	flag.StringVar(&env.ThirdPartyDir, "thirdpartydir", "", "third-party directory (replaces ${POMDOG_THIRDPARTY_DIR})")
	flag.StringVar(&env.OutNinjaFile, "outninja", "", "output ninja file (build.ninja)")
	flag.StringVar(&env.OutDir, "outdir", "", "output content directory")
	flag.StringVar(&env.IntDir, "intdir", "", "intermediate directory")
	flag.StringVar(&env.ToolDir, "tooldir", "", "tool directory")
	flag.Parse()

	if env.InRecipe == "" {
		log.Fatal("-recipe is required")
	}
	if env.OutNinjaFile == "" {
		log.Fatal("-outninja is required")
	}
	if env.OutDir == "" {
		log.Fatal("-outdir is required")
	}
	if env.ToolDir == "" {
		log.Fatal("-tooldir is required")
	}

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

// ConvertRecipe represents the TOML recipe for asset conversion.
type ConvertRecipe struct {
	GameControllerDB []*GameControllerDBEntry `toml:"game_controller_db"`
}

// GameControllerDBEntry defines a conversion from SDL_GameControllerDB to .gcdb binary.
type GameControllerDBEntry struct {
	In              string `toml:"in"`
	Out             string `toml:"out"`
	UseAbsPathInDir bool   `toml:"use_abs_path_indir"`
}

func run(env *Env) error {
	if abs, err := filepath.Abs(env.OutDir); err == nil {
		env.OutDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutNinjaFile); err == nil {
		env.OutNinjaFile = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.ToolDir); err == nil {
		env.ToolDir = filepath.Clean(abs)
	}
	if env.InDir != "" {
		if abs, err := filepath.Abs(env.InDir); err == nil {
			env.InDir = filepath.Clean(abs)
		}
	}
	if env.IntDir != "" {
		if abs, err := filepath.Abs(env.IntDir); err == nil {
			env.IntDir = filepath.Clean(abs)
		}
	}
	if env.ThirdPartyDir != "" {
		if abs, err := filepath.Abs(env.ThirdPartyDir); err == nil {
			env.ThirdPartyDir = filepath.Clean(abs)
		}
	}

	recipe, err := readRecipe(env.InRecipe, env.ThirdPartyDir)
	if err != nil {
		return fmt.Errorf("readRecipe() failed: %w", err)
	}

	gen := &ninja.Generator{}

	// NOTE: Game controller DB conversion rule
	gcdbExe := filepath.Join(env.ToolDir, "generate-game-controller-db")
	if runtime.GOOS == "windows" {
		gcdbExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("generate_game_controller_db_exe", gcdbExe))
	gen.AddRule(&ninja.Rule{
		Name:    "generate_game_controller_db",
		Command: "$generate_game_controller_db_exe -i $in -o $out",
	})

	// NOTE: Track output directories to create
	outDirs := map[string]struct{}{}

	for _, entry := range recipe.GameControllerDB {
		inPath := entry.In
		if entry.UseAbsPathInDir {
			// NOTE: Input path is already absolute (e.g. from thirdparty dir)
		} else if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		outPath := filepath.Join(env.OutDir, entry.Out)
		outDirs[filepath.Dir(outPath)] = struct{}{}

		gen.AddBuild(&ninja.Build{
			Rule:    "generate_game_controller_db",
			OutFile: outPath,
			InFiles: []string{inPath},
		})
	}

	// NOTE: Create output directories
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

	// NOTE: Create directory for ninja file
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

func readRecipe(inFile string, thirdPartyDir string) (*ConvertRecipe, error) {
	buf, err := os.ReadFile(inFile)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	// NOTE: Substitute environment variables
	content := string(buf)
	if thirdPartyDir != "" {
		thirdPartyDir = filepath.ToSlash(filepath.Clean(thirdPartyDir))
		content = strings.ReplaceAll(content, "${POMDOG_THIRDPARTY_DIR}", thirdPartyDir)
	}

	recipe := &ConvertRecipe{}
	if err := toml.Unmarshal([]byte(content), recipe); err != nil {
		return nil, fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	if err := validateRecipe(recipe); err != nil {
		return nil, fmt.Errorf("validateRecipe() failed: %w", err)
	}

	return recipe, nil
}

func validateRecipe(recipe *ConvertRecipe) error {
	for i, entry := range recipe.GameControllerDB {
		if entry.In == "" {
			return fmt.Errorf("[[game_controller_db]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[game_controller_db]] #%d: out must not be empty", i)
		}
	}
	return nil
}
