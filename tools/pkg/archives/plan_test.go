// Copyright mogemimi. Distributed under the MIT license.

package archives

import (
	"path/filepath"
	"slices"
	"strings"
	"testing"
)

func readTestPlan(t *testing.T, settingsContent, recipeContent string, platforms ...string) (*ArchivePlan, error) {
	t.Helper()

	settings := ArchiveSettings{}
	if err := settings.ReadFile(writeRecipeFile(t, settingsContent)); err != nil {
		t.Fatalf("settings.ReadFile() failed: %v", err)
	}
	recipe := ArchiveRecipe{}
	if err := recipe.ReadFile(writeRecipeFile(t, recipeContent)); err != nil {
		t.Fatalf("recipe.ReadFile() failed: %v", err)
	}
	return PlanArchive(&settings, &recipe, platforms)
}

func TestPlanArchive(t *testing.T) {
	plan, err := readTestPlan(t, testSettings+`
[[strategy.zstd.dictionary]]
name = "unused"
max_size = 1024
paths = ["**/*.xml"]

[[strategy.zstd.dictionary]]
name = "text"
max_size = 4096
paths = ["shaders/webgl/*.glsl", "**/*.json"]
`, `[[group]]
paths = ["particles/fire.json", "fonts/a.ttf"]

[[group]]
compression = "none"
paths = ["config.json"]

[[group]]
target_platforms = ["web"]
paths = ["shaders/webgl/a_vs.glsl"]

[[group]]
target_platforms = ["windows"]
paths = ["shaders/d3d11/a_vs.dxbc"]
`, PlatformWeb)
	if err != nil {
		t.Fatalf("PlanArchive() failed: %v", err)
	}

	if plan.Strategy.Compression != CompressionZstd {
		t.Errorf("strategy compression = %q, want %q", plan.Strategy.Compression, CompressionZstd)
	}

	type plannedFile struct {
		path        string
		compression string
		dictionary  string
	}
	got := []plannedFile{}
	for _, f := range plan.Files {
		dictionary := ""
		if f.Dictionary != nil {
			dictionary = f.Dictionary.Settings.Name
		}
		got = append(got, plannedFile{f.Path, f.Compression, dictionary})
	}
	want := []plannedFile{
		{"config.json", CompressionNone, ""},
		{"fonts/a.ttf", CompressionZstd, ""},
		{"particles/fire.json", CompressionZstd, "text"},
		{"shaders/webgl/a_vs.glsl", CompressionZstd, "text"},
	}
	if !slices.Equal(got, want) {
		t.Errorf("files = %+v, want %+v", got, want)
	}

	if len(plan.Dictionaries) != 1 {
		t.Fatalf("got %d dictionaries, want 1", len(plan.Dictionaries))
	}
	d := plan.Dictionaries[0]
	if d.Settings.Name != "text" || d.ID != 2 {
		t.Errorf("dictionary = %q with ID %d, want \"text\" with ID 2", d.Settings.Name, d.ID)
	}
	if want := []string{"particles/fire.json", "shaders/webgl/a_vs.glsl"}; !slices.Equal(d.Files, want) {
		t.Errorf("dictionary files = %v, want %v", d.Files, want)
	}
}

func TestPlanArchiveDesktop(t *testing.T) {
	plan, err := readTestPlan(t, testSettings, `[[group]]
paths = ["fonts/a.ttf"]

[[group]]
target_platforms = ["web"]
paths = ["shaders/webgl/a_vs.glsl"]

[[group]]
target_platforms = ["windows", "linux"]
paths = ["shaders/vk/a_vs.spv"]
`, PlatformWindows, PlatformLinux)
	if err != nil {
		t.Fatalf("PlanArchive() failed: %v", err)
	}

	paths := []string{}
	for _, f := range plan.Files {
		paths = append(paths, f.Path)
		if f.Compression != CompressionNone || f.Dictionary != nil {
			t.Errorf("%s: compression %q with dictionary %v, want none", f.Path, f.Compression, f.Dictionary)
		}
	}
	if want := []string{"fonts/a.ttf", "shaders/vk/a_vs.spv"}; !slices.Equal(paths, want) {
		t.Errorf("files = %v, want %v", paths, want)
	}
	if len(plan.Dictionaries) != 0 {
		t.Errorf("got %d dictionaries, want none", len(plan.Dictionaries))
	}
}

func TestPlanArchiveInvalid(t *testing.T) {
	for _, tc := range []struct {
		name      string
		settings  string
		recipe    string
		platforms []string
		want      string
	}{
		{"different strategies", testSettings, `[[group]]
paths = ["a.txt"]
`, []string{PlatformWindows, PlatformWeb}, "platforms windows, web use different strategies"},
		{"group zstd without settings", `[[strategy]]
target_platforms = ["windows"]
compression = "none"
`, `[[group]]
compression = "zstd"
paths = ["a.txt"]
`, []string{PlatformWindows}, `has no [strategy.zstd] settings for the recipe group with "a.txt"`},
		{"duplicate path", testSettings, `[[group]]
paths = ["a.txt"]

[[group]]
paths = ["a.txt"]
`, []string{PlatformWeb}, "already exists: a.txt"},
		{"overlapping dictionaries", testSettings + `
[[strategy.zstd.dictionary]]
name = "json"
max_size = 1024
paths = ["**/*.json"]

[[strategy.zstd.dictionary]]
name = "particles"
max_size = 1024
paths = ["particles/**"]
`, `[[group]]
paths = ["particles/fire.json"]
`, []string{PlatformWeb}, `particles/fire.json matches both dictionaries "json" and "particles"`},
	} {
		t.Run(tc.name, func(t *testing.T) {
			_, err := readTestPlan(t, tc.settings, tc.recipe, tc.platforms...)
			if err == nil {
				t.Fatal("PlanArchive() succeeded, want an error")
			}
			if !strings.Contains(err.Error(), tc.want) {
				t.Errorf("error %q does not contain %q", err, tc.want)
			}
		})
	}
}

func TestZstdIntermediatePaths(t *testing.T) {
	if got, want := ZstdFramePath("zstd", "shaders/webgl/a_vs.glsl"), "zstd/files/shaders/webgl/a_vs.glsl.zst"; got != filepath.FromSlash(want) {
		t.Errorf("ZstdFramePath() = %q, want %q", got, want)
	}
	if got, want := ZstdDictionaryPath("zstd", 12), "zstd/dictionaries/12.dict"; got != filepath.FromSlash(want) {
		t.Errorf("ZstdDictionaryPath() = %q, want %q", got, want)
	}
}
