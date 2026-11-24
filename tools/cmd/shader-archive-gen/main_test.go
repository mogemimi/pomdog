// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"os"
	"path/filepath"
	"slices"
	"strings"
	"testing"
	"time"

	archives "github.com/mogemimi/pomdog/tools/pkg/archives"
)

func TestWriteIfChanged(t *testing.T) {
	dir := t.TempDir()
	recipe := filepath.Join(dir, "shaderbuild.toml")
	writeRecipe := func(content string) {
		t.Helper()
		if err := os.WriteFile(recipe, []byte(content), 0o644); err != nil {
			t.Fatalf("os.WriteFile() failed: %v", err)
		}
	}
	env := &Env{
		OutFile:        filepath.Join(dir, "out", "archive_shaders.toml"),
		WriteIfChanged: true,
	}

	writeRecipe("[[compile_vs]]\nsource = \"sprite_vs.hlsl\"\n")
	if err := run(env, []string{recipe}); err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	// NOTE: Move the modification time to the past to detect a rewrite.
	past := time.Now().Add(-time.Hour).Truncate(time.Second)
	if err := os.Chtimes(env.OutFile, past, past); err != nil {
		t.Fatalf("os.Chtimes() failed: %v", err)
	}
	modTime := func() time.Time {
		t.Helper()
		stat, err := os.Stat(env.OutFile)
		if err != nil {
			t.Fatalf("os.Stat() failed: %v", err)
		}
		return stat.ModTime()
	}

	if err := run(env, []string{recipe}); err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	if !modTime().Equal(past) {
		t.Error("the unchanged output file was rewritten")
	}

	writeRecipe("[[compile_vs]]\nsource = \"sprite_vs.hlsl\"\n\n[[compile_ps]]\nsource = \"sprite_ps.hlsl\"\n")
	if err := run(env, []string{recipe}); err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	if modTime().Equal(past) {
		t.Error("the changed output file was not rewritten")
	}
	buf, err := os.ReadFile(env.OutFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	if !strings.Contains(string(buf), "shaders/webgl/sprite_ps.glsl") {
		t.Errorf("the output file does not list the new shader:\n%s", buf)
	}
}

func TestMetalLibraryGroup(t *testing.T) {
	dir := t.TempDir()
	recipeFile := filepath.Join(dir, "shaderbuild.toml")
	content := "[[compile_vs]]\nsource = \"sprite.slang\"\nname = \"sprite_vs\"\n\n[[compile_ps]]\nsource = \"sprite.slang\"\nname = \"sprite_ps\"\n"
	if err := os.WriteFile(recipeFile, []byte(content), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	env := &Env{
		OutFile: filepath.Join(dir, "archive_shaders.toml"),
	}
	if err := run(env, []string{recipeFile}); err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	recipe := &archives.ArchiveRecipe{}
	if err := recipe.ReadFile(env.OutFile); err != nil {
		t.Fatalf("recipe.ReadFile() failed: %v", err)
	}

	var metalPaths []string
	for _, group := range recipe.Group {
		for _, path := range group.Paths {
			if !strings.HasPrefix(path, "shaders/metal/") {
				continue
			}
			if !slices.Equal(group.TargetPlatforms, []string{archives.PlatformMacOS}) {
				t.Errorf("%q targets %v, want [%s]", path, group.TargetPlatforms, archives.PlatformMacOS)
			}
			metalPaths = append(metalPaths, path)
		}
	}

	want := []string{"shaders/metal/sprite_ps.metallib", "shaders/metal/sprite_vs.metallib"}
	if !slices.Equal(metalPaths, want) {
		t.Errorf("Metal paths = %v, want %v", metalPaths, want)
	}
}
