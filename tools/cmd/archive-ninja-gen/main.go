// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"strings"

	ninja "github.com/mogemimi/pomdog/tools/pkg/ninja"
)

type Env struct {
	OutNinjaFile string
	ContentDir   string
	OutDir       string
	IntDir       string
	ToolDir      string
	DepSubninjas stringSlice // sub-ninja files whose outputs are order-only deps
	Recipes      []string    // positional args: archive*.toml files
}

// stringSlice implements flag.Value for repeated string flags.
type stringSlice []string

func (s *stringSlice) String() string { return strings.Join(*s, ", ") }
func (s *stringSlice) Set(v string) error {
	*s = append(*s, v)
	return nil
}

func main() {
	env := &Env{}
	flag.StringVar(&env.OutNinjaFile, "outninja", "", "output ninja file (build.ninja)")
	flag.StringVar(&env.ContentDir, "contentdir", "", "content directory")
	flag.StringVar(&env.OutDir, "outdir", "", "output shipping directory")
	flag.StringVar(&env.IntDir, "intdir", "", "intermediate directory")
	flag.StringVar(&env.ToolDir, "tooldir", "", "tool directory")
	flag.Var(&env.DepSubninjas, "dep-subninja", "sub-ninja file whose build outputs become order-only deps (repeatable)")
	flag.Parse()

	env.Recipes = flag.Args()

	if env.OutNinjaFile == "" {
		log.Fatal("-outninja is required")
	}
	if env.ContentDir == "" {
		log.Fatal("-contentdir is required")
	}
	if env.OutDir == "" {
		log.Fatal("-outdir is required")
	}
	if env.IntDir == "" {
		log.Fatal("-intdir is required")
	}
	if env.ToolDir == "" {
		log.Fatal("-tooldir is required")
	}
	if len(env.Recipes) == 0 {
		log.Fatal("at least one archive recipe file (*.toml) must be specified as a positional argument")
	}

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

func run(env *Env) error {
	if abs, err := filepath.Abs(env.ContentDir); err == nil {
		env.ContentDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutDir); err == nil {
		env.OutDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.IntDir); err == nil {
		env.IntDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutNinjaFile); err == nil {
		env.OutNinjaFile = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.ToolDir); err == nil {
		env.ToolDir = filepath.Clean(abs)
	}

	absRecipes := make([]string, 0, len(env.Recipes))
	for _, r := range env.Recipes {
		if abs, err := filepath.Abs(r); err == nil {
			absRecipes = append(absRecipes, filepath.Clean(abs))
		} else {
			absRecipes = append(absRecipes, r)
		}
	}
	env.Recipes = absRecipes

	// Parse order-only deps from sub-ninja files
	var orderOnlyDeps []string
	for _, subNinja := range env.DepSubninjas {
		outputs, err := ninja.ParseBuildOutputs(subNinja)
		if err != nil {
			return fmt.Errorf("ParseBuildOutputs(%q) failed: %w", subNinja, err)
		}
		orderOnlyDeps = append(orderOnlyDeps, outputs...)
	}

	gen := &ninja.Generator{}

	archiveExe := filepath.Join(env.ToolDir, "archive-content")
	if runtime.GOOS == "windows" {
		archiveExe += ".exe"
	}

	gen.AddVariable(ninja.NewVariableAsPath("archive_content_exe", archiveExe))

	// Rule for desktop archive
	gen.AddRule(&ninja.Rule{
		Name:    "archive_desktop",
		Command: "$archive_content_exe --contentdir $contentdir -o $out --outbin $outbin --outdebug $outdebug --depfile $depfile $recipes",
		DepFile: "$depfile",
		Deps:    "gcc",
	})

	// Rule for web (emscripten) archive
	gen.AddRule(&ninja.Rule{
		Name:    "archive_web",
		Command: "$archive_content_exe --platform emscripten --contentdir $contentdir -o $out --outbin $outbin --outdebug $outdebug --depfile $depfile $recipes",
		DepFile: "$depfile",
		Deps:    "gcc",
	})

	recipesStr := strings.Join(env.Recipes, " ")

	// Desktop archive build
	{
		desktopDir := filepath.Join(env.OutDir, "desktop")
		desktopDebugDir := filepath.Join(env.IntDir, "desktop")
		outIdx := filepath.Join(desktopDir, "content.idx")
		outPak := filepath.Join(desktopDir, "content.pak")
		outDebug := filepath.Join(desktopDebugDir, "content.idx-debug")
		depFile := filepath.Join(env.IntDir, "desktop.d")

		gen.AddBuild(&ninja.Build{
			Rule:             "archive_desktop",
			OutFile:          outIdx,
			ImplicitOutFiles: []string{outPak},
			InFiles:          env.Recipes,
			OrderOnlyDeps:    orderOnlyDeps,
			Variables: []*ninja.Variable{
				ninja.NewVariableAsPath("contentdir", env.ContentDir),
				ninja.NewVariableAsPath("outbin", outPak),
				ninja.NewVariableAsPath("outdebug", outDebug),
				ninja.NewVariableAsPath("depfile", depFile),
				ninja.NewVariableAsString("recipes", recipesStr),
			},
		})

		if err := os.MkdirAll(desktopDir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
		if err := os.MkdirAll(desktopDebugDir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	// Web archive build
	{
		webDir := filepath.Join(env.OutDir, "web")
		webDebugDir := filepath.Join(env.IntDir, "web")
		outIdx := filepath.Join(webDir, "content.idx")
		outPak := filepath.Join(webDir, "content.pak")
		outDebug := filepath.Join(webDebugDir, "content.idx-debug")
		depFile := filepath.Join(env.IntDir, "web.d")

		gen.AddBuild(&ninja.Build{
			Rule:             "archive_web",
			OutFile:          outIdx,
			ImplicitOutFiles: []string{outPak},
			InFiles:          env.Recipes,
			OrderOnlyDeps:    orderOnlyDeps,
			Variables: []*ninja.Variable{
				ninja.NewVariableAsPath("contentdir", env.ContentDir),
				ninja.NewVariableAsPath("outbin", outPak),
				ninja.NewVariableAsPath("outdebug", outDebug),
				ninja.NewVariableAsPath("depfile", depFile),
				ninja.NewVariableAsString("recipes", recipesStr),
			},
		})

		if err := os.MkdirAll(webDir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
		if err := os.MkdirAll(webDebugDir, os.ModePerm); err != nil {
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
