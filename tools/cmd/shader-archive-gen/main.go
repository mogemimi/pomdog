// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"sort"
	"strings"

	archives "github.com/mogemimi/pomdog/tools/pkg/archives"
	toml "github.com/pelletier/go-toml/v2"
)

func main() {
	env := Env{}
	flag.StringVar(&env.OutFile, "o", "", "output archive recipe file (*.toml)")
	flag.Parse()

	if len(flag.Args()) == 0 {
		log.Fatal("at least one shaderbuild.toml must be specified as a positional argument")
	}
	if env.OutFile == "" {
		log.Fatal("-o is required")
	}

	if err := run(&env, flag.Args()); err != nil {
		log.Fatal(err)
	}
}

type Env struct {
	OutFile string
}

// ShaderBuildRecipe mirrors the [[builds]] section of shaderbuild.toml.
type ShaderBuildRecipe struct {
	Builds []*ShaderBuild `toml:"builds"`
}

type ShaderBuild struct {
	Source     string `toml:"source"`
	Name       string `toml:"name"`
	Stage      string `toml:"stage"`
	EntryPoint string `toml:"entrypoint"`
}

func readShaderBuildRecipe(file string) (*ShaderBuildRecipe, error) {
	buf, err := os.ReadFile(file)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	recipe := &ShaderBuildRecipe{}
	if err := toml.Unmarshal(buf, recipe); err != nil {
		return nil, fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	// NOTE: Complement missing fields.
	for _, build := range recipe.Builds {
		if len(build.Name) == 0 {
			ext := filepath.Ext(build.Source)
			build.Name = strings.TrimSuffix(build.Source, ext)
		}
		if len(build.Stage) == 0 {
			if strings.HasSuffix(build.Name, "_vs") {
				build.Stage = "vs"
			} else if strings.HasSuffix(build.Name, "_ps") {
				build.Stage = "ps"
			} else if strings.HasSuffix(build.Name, "_cs") {
				build.Stage = "cs"
			}
		}
	}

	return recipe, nil
}

func run(env *Env, recipeFiles []string) error {
	// NOTE: Collect all shader build names from all recipe files.
	var allBuilds []*ShaderBuild
	for _, file := range recipeFiles {
		recipe, err := readShaderBuildRecipe(file)
		if err != nil {
			return fmt.Errorf("readShaderBuildRecipe(%q): %w", file, err)
		}
		allBuilds = append(allBuilds, recipe.Builds...)
	}

	// NOTE: Collect shader names (sorted for deterministic output).
	names := make([]string, 0, len(allBuilds))
	for _, build := range allBuilds {
		names = append(names, build.Name)
	}
	sort.Strings(names)

	// NOTE: Generate archive recipe groups.
	archiveRecipe := &archives.ArchiveRecipe{}

	// NOTE: Group 1: reflect files (all platforms).
	{
		group := &archives.ArchiveGroup{}
		for _, name := range names {
			group.Paths = append(group.Paths, fmt.Sprintf("shaders/reflect/%s.reflect", name))
		}
		archiveRecipe.Group = append(archiveRecipe.Group, group)
	}

	// NOTE: Group 2: GLSL desktop (windows, linux, macos).
	{
		group := &archives.ArchiveGroup{
			TargetPlatforms: []string{"windows", "linux", "macos"},
		}
		for _, name := range names {
			group.Paths = append(group.Paths, fmt.Sprintf("shaders/glsl/%s.glsl", name))
		}
		archiveRecipe.Group = append(archiveRecipe.Group, group)
	}

	// NOTE: Group 3: GLSL ES / WebGL (emscripten).
	{
		group := &archives.ArchiveGroup{
			TargetPlatforms: []string{"emscripten"},
		}
		for _, name := range names {
			group.Paths = append(group.Paths, fmt.Sprintf("shaders/webgl/%s.glsl", name))
		}
		archiveRecipe.Group = append(archiveRecipe.Group, group)
	}

	// NOTE: Group 4: DXBC (windows only).
	{
		group := &archives.ArchiveGroup{
			TargetPlatforms: []string{"windows"},
		}
		for _, name := range names {
			group.Paths = append(group.Paths, fmt.Sprintf("shaders/d3d11/%s.dxbc", name))
		}
		archiveRecipe.Group = append(archiveRecipe.Group, group)
	}

	// NOTE: Encode and write.
	content, err := archiveRecipe.Encode()
	if err != nil {
		return fmt.Errorf("archiveRecipe.Encode() failed: %w", err)
	}

	if dir := filepath.Dir(env.OutFile); len(dir) > 0 {
		if err := os.MkdirAll(dir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	if err := os.WriteFile(env.OutFile, []byte(content), 0o644); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}

	return nil
}
