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

// shaderBuildRecipeFile is the on-disk TOML representation of shaderbuild.toml.
type shaderBuildRecipeFile struct {
	CompileVS []*shaderBuildEntry `toml:"compile_vs"`
	CompilePS []*shaderBuildEntry `toml:"compile_ps"`
}

type shaderBuildEntry struct {
	Source string `toml:"source"`
	Name   string `toml:"name"`
}

type ShaderBuild struct {
	Name  string
	Stage string
}

func readShaderBuildRecipe(file string) ([]*ShaderBuild, error) {
	buf, err := os.ReadFile(file)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	var rf shaderBuildRecipeFile
	if err := toml.Unmarshal(buf, &rf); err != nil {
		return nil, fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	var builds []*ShaderBuild
	for _, vs := range rf.CompileVS {
		name := vs.Name
		if len(name) == 0 {
			ext := filepath.Ext(vs.Source)
			name = strings.TrimSuffix(vs.Source, ext)
		}
		builds = append(builds, &ShaderBuild{Name: name, Stage: "vs"})
	}
	for _, ps := range rf.CompilePS {
		name := ps.Name
		if len(name) == 0 {
			ext := filepath.Ext(ps.Source)
			name = strings.TrimSuffix(ps.Source, ext)
		}
		builds = append(builds, &ShaderBuild{Name: name, Stage: "ps"})
	}

	return builds, nil
}

func run(env *Env, recipeFiles []string) error {
	// NOTE: Collect all shader build names from all recipe files.
	var allBuilds []*ShaderBuild
	for _, file := range recipeFiles {
		builds, err := readShaderBuildRecipe(file)
		if err != nil {
			return fmt.Errorf("readShaderBuildRecipe(%q): %w", file, err)
		}
		allBuilds = append(allBuilds, builds...)
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

	// NOTE: Group 5: SPIR-V for Vulkan (windows, linux).
	{
		group := &archives.ArchiveGroup{
			TargetPlatforms: []string{"windows", "linux"},
		}
		for _, name := range names {
			group.Paths = append(group.Paths, fmt.Sprintf("shaders/vk/%s.spv", name))
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
