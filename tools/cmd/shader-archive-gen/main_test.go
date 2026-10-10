// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"
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
