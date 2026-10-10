// Copyright mogemimi. Distributed under the MIT license.

package archives

import (
	"os"
	"path/filepath"
	"strings"
	"testing"
)

func writeRecipeFile(t *testing.T, content string) string {
	t.Helper()
	f := filepath.Join(t.TempDir(), "archive.toml")
	if err := os.WriteFile(f, []byte(content), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	return f
}

func TestReadFileCompression(t *testing.T) {
	f := writeRecipeFile(t, `[[group]]
paths = ["a.json"]

[[group]]
compression = "none"
paths = ["b.png"]

[[group]]
target_platforms = ["web"]
compression = "zstd"
paths = ["c.glsl"]
`)

	recipe := ArchiveRecipe{}
	if err := recipe.ReadFile(f); err != nil {
		t.Fatalf("ReadFile() failed: %v", err)
	}

	expected := []string{"", CompressionNone, CompressionZstd}
	if len(recipe.Group) != len(expected) {
		t.Fatalf("got %d groups, want %d", len(recipe.Group), len(expected))
	}
	for i, group := range recipe.Group {
		if group.Compression != expected[i] {
			t.Errorf("group %d: got compression %q, want %q", i+1, group.Compression, expected[i])
		}
	}
}

func TestReadFileUnknownCompression(t *testing.T) {
	f := writeRecipeFile(t, `[[group]]
paths = ["a.json"]

[[group]]
compression = "lz4"
paths = ["b.json"]
`)

	recipe := ArchiveRecipe{}
	err := recipe.ReadFile(f)
	if err == nil {
		t.Fatal("ReadFile() succeeded, want an error for an unknown compression method")
	}
	if !strings.Contains(err.Error(), "group 2") || !strings.Contains(err.Error(), `"lz4"`) {
		t.Errorf("error does not name the group and method: %v", err)
	}
}

func TestReadFileUnknownPlatform(t *testing.T) {
	f := writeRecipeFile(t, `[[group]]
target_platforms = ["windows", "emscripten"]
paths = ["a.json"]
`)

	recipe := ArchiveRecipe{}
	err := recipe.ReadFile(f)
	if err == nil {
		t.Fatal("ReadFile() succeeded, want an error for an unknown platform")
	}
	if !strings.Contains(err.Error(), "group 1") || !strings.Contains(err.Error(), `"emscripten"`) {
		t.Errorf("error does not name the group and platform: %v", err)
	}
}

func TestReadFileUnknownKey(t *testing.T) {
	f := writeRecipeFile(t, `[[group]]
compresion = "zstd"
paths = ["a.json"]
`)

	recipe := ArchiveRecipe{}
	err := recipe.ReadFile(f)
	if err == nil {
		t.Fatal("ReadFile() succeeded, want an error for a misspelled key")
	}
	if !strings.Contains(err.Error(), "compresion") {
		t.Errorf("error does not name the unknown key: %v", err)
	}
}

func TestEncodeCompression(t *testing.T) {
	recipe := ArchiveRecipe{
		Group: []*ArchiveGroup{
			{Paths: []string{"a.json"}},
			{Compression: CompressionZstd, Paths: []string{"b.json"}},
		},
	}

	s, err := recipe.Encode()
	if err != nil {
		t.Fatalf("Encode() failed: %v", err)
	}
	if strings.Count(s, "compression") != 1 || !strings.Contains(s, `compression = "zstd"`) {
		t.Errorf("unexpected encoding:\n%s", s)
	}

	f := writeRecipeFile(t, s)
	decoded := ArchiveRecipe{}
	if err := decoded.ReadFile(f); err != nil {
		t.Fatalf("ReadFile() failed: %v", err)
	}
	if decoded.Group[0].Compression != "" || decoded.Group[1].Compression != CompressionZstd {
		t.Errorf("compression did not round-trip: %q, %q", decoded.Group[0].Compression, decoded.Group[1].Compression)
	}
}
