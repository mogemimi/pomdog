// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"os"
	"path/filepath"
	"runtime"
	"slices"
	"strings"
	"testing"
	"time"
)

const testZstdSettings = `[[strategy]]
target_platforms = ["windows", "macos", "linux"]
compression = "none"

[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 22
min_saved_bytes = 32
max_ratio_percent = 95

[[strategy.zstd.dictionary]]
name = "text"
max_size = 4096
paths = ["**/*.json"]
`

const testRecipe = `[[group]]
paths = ["a.json", "b.json", "fonts/c.ttf"]

[[group]]
compression = "none"
paths = ["d.json"]
`

// newTestEnv writes the settings and a recipe into a temporary directory and
// returns an Env that generates the archive build into the same directory.
func newTestEnv(t *testing.T, settings, recipe string) *Env {
	t.Helper()
	dir := t.TempDir()

	settingsFile := filepath.Join(dir, "archive_settings.toml")
	if err := os.WriteFile(settingsFile, []byte(settings), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	recipeFile := filepath.Join(dir, "archive.toml")
	if err := os.WriteFile(recipeFile, []byte(recipe), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}

	return &Env{
		OutNinjaFile: filepath.Join(dir, "archivebuild", "build.ninja"),
		ContentDir:   filepath.Join(dir, "content"),
		OutDir:       filepath.Join(dir, "shipping"),
		IntDir:       filepath.Join(dir, "archivebuild"),
		ToolDir:      filepath.Join(dir, "tools"),
		SettingsFile: settingsFile,
		Recipes:      []string{recipeFile},
	}
}

// readNinjaBuild runs archive-ninja-gen and returns the generated Ninja file
// with forward slashes.
func readNinjaBuild(t *testing.T, env *Env) string {
	t.Helper()
	if err := run(env); err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	buf, err := os.ReadFile(env.OutNinjaFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	return string(buf)
}

// findBuild returns the build statement that produces out, including its variables.
func findBuild(t *testing.T, content, out string) string {
	t.Helper()
	for _, statement := range strings.Split(content, "\nbuild ")[1:] {
		// NOTE: Escaped colons in paths are `$:`, so the first ": " ends the outputs.
		outputs, _, _ := strings.Cut(statement, ": ")
		for _, output := range strings.Fields(outputs) {
			if strings.HasSuffix(output, out) {
				// NOTE: Splitting removed the line break that ends the statement.
				return statement + "\n"
			}
		}
	}
	t.Fatalf("no build statement for %s:\n%s", out, content)
	return ""
}

// implicitInputs returns the implicit inputs of a statement from findBuild.
func implicitInputs(statement string) []string {
	// NOTE: Join the continued lines of the build line.
	line, _, _ := strings.Cut(strings.ReplaceAll(statement, "$\n", ""), "\n")
	_, inputs, _ := strings.Cut(line, " | ")
	inputs, _, _ = strings.Cut(inputs, " || ")
	return strings.Fields(inputs)
}

// toolPath returns the escaped path of the executable name in the tool directory.
func toolPath(env *Env, name string) string {
	path := filepath.Join(env.ToolDir, name)
	if runtime.GOOS == "windows" {
		path += ".exe"
	}
	return escapeTestPath(path)
}

func TestRunPassesSettings(t *testing.T) {
	env := newTestEnv(t, `[[strategy]]
target_platforms = ["windows", "macos", "linux", "web"]
compression = "none"
`, testRecipe)
	content := readNinjaBuild(t, env)

	if n := strings.Count(content, "--settings $archive_settings"); n != 1 {
		t.Errorf("%d rules pass the settings, want 1:\n%s", n, content)
	}

	// NOTE: Each archive build lists the settings file as an implicit input,
	// and the `archive_settings` variable names it once more.
	builds := strings.Count(content, "\nbuild ")
	if builds < 2 {
		t.Fatalf("%d archive builds, want at least the host and web archives:\n%s", builds, content)
	}
	if n := strings.Count(content, "archive_settings.toml"); n != builds+1 {
		t.Errorf("the settings file appears %d times, want %d:\n%s", n, builds+1, content)
	}

	// NOTE: Without zstd, the build needs no zstd command.
	if strings.Contains(content, "zstd_exe") {
		t.Errorf("the build uses zstd:\n%s", content)
	}
}

func TestRunMissingStrategy(t *testing.T) {
	env := newTestEnv(t, `[[strategy]]
target_platforms = ["windows", "macos", "linux"]
compression = "none"
`, testRecipe)
	err := run(env)
	if err == nil {
		t.Fatal("run() succeeded, want an error for the web archive without a strategy")
	}
	if !strings.Contains(err.Error(), `"web"`) {
		t.Errorf("error does not name the platform: %v", err)
	}
}

func TestRunZstd(t *testing.T) {
	env := newTestEnv(t, testZstdSettings, testRecipe)
	content := filepath.ToSlash(readNinjaBuild(t, env))

	// NOTE: The build runs the zstd command in the tool directory through
	// zstd-wrapper, which gives each output the time when zstd wrote it.
	zstd := toolPath(env, "zstd")
	zstdWrapper := toolPath(env, "zstd-wrapper")
	archiveContent := toolPath(env, "archive-content")
	if !strings.Contains(content, "zstd_exe = "+zstd+"\n") ||
		!strings.Contains(content, "zstd_wrapper_exe = "+zstdWrapper+"\n") {
		t.Errorf("the build does not run %s through %s:\n%s", zstd, zstdWrapper, content)
	}
	if n := strings.Count(content, "command = $zstd_wrapper_exe $zstd_exe "); n != 3 {
		t.Errorf("%d rules run zstd through zstd-wrapper, want 3:\n%s", n, content)
	}
	// NOTE: zstd-wrapper does not change the data, so no build depends on it,
	// and only its variable names it.
	if strings.Contains(content, "restat") || strings.Count(content, zstdWrapper) != 1 {
		t.Errorf("the build uses restat or depends on zstd-wrapper:\n%s", content)
	}
	dictionary := escapeTestPath(filepath.Join(env.IntDir, "web", "zstd", "dictionaries", "1.dict"))

	// NOTE: The dictionary is trained on the zstd files that match its paths.
	train := findBuild(t, content, "web/zstd/dictionaries/1.dict")
	if !strings.Contains(train, ": zstd_train") ||
		!strings.Contains(train, "content/a.json") ||
		!strings.Contains(train, "content/b.json") ||
		strings.Contains(train, "content/d.json") ||
		!strings.Contains(train, "zstd_level = --ultra -22\n") ||
		!strings.Contains(train, "max_size = 4096\n") ||
		!strings.Contains(train, "dictionary_id = 1\n") ||
		!strings.Contains(train, "web/zstd/dictionaries/1.filelist\n") ||
		!slices.Contains(implicitInputs(train), zstd) {
		t.Errorf("unexpected training build:\n%s", train)
	}

	// NOTE: The file list ends lines with LF on every platform for zstd --filelist.
	fileList := filepath.Join(env.IntDir, "web", "zstd", "dictionaries", "1.filelist")
	buf, err := os.ReadFile(fileList)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	contentDir := filepath.ToSlash(env.ContentDir)
	if want := contentDir + "/a.json\n" + contentDir + "/b.json\n"; string(buf) != want {
		t.Errorf("file list is %q, want %q", buf, want)
	}

	// NOTE: Generating the build again keeps the unchanged file list.
	past := time.Now().Add(-time.Hour).Truncate(time.Second)
	if err := os.Chtimes(fileList, past, past); err != nil {
		t.Fatalf("os.Chtimes() failed: %v", err)
	}
	readNinjaBuild(t, env)
	if stat, err := os.Stat(fileList); err != nil || !stat.ModTime().Equal(past) {
		t.Errorf("the unchanged file list was rewritten: %v", err)
	}

	for _, tc := range []struct {
		path string
		rule string
	}{
		{"a.json", "zstd_compress_dictionary"},
		{"b.json", "zstd_compress_dictionary"},
		{"fonts/c.ttf", "zstd_compress"},
	} {
		build := findBuild(t, content, "web/zstd/files/"+tc.path+".zst")
		if !strings.Contains(build, ": "+tc.rule+" ") || !strings.Contains(build, "content/"+tc.path) {
			t.Errorf("%s: unexpected build:\n%s", tc.path, build)
		}
		if !slices.Contains(implicitInputs(build), zstd) {
			t.Errorf("%s: the build does not depend on the zstd executable:\n%s", tc.path, build)
		}
		if tc.rule == "zstd_compress_dictionary" && !slices.Contains(implicitInputs(build), dictionary) {
			t.Errorf("%s: the build does not depend on the dictionary:\n%s", tc.path, build)
		}
	}
	if strings.Contains(content, "d.json.zst") {
		t.Errorf("d.json requests no compression, but the build compresses it:\n%s", content)
	}

	// NOTE: The archive depends on the content files, frames, and dictionary.
	archive := findBuild(t, content, "shipping/web/content.idx")
	for _, input := range []string{
		"content/a.json", "content/d.json", "content/fonts/c.ttf",
		"web/zstd/files/a.json.zst", "web/zstd/files/fonts/c.ttf.zst",
		"web/zstd/dictionaries/1.dict", "archive_settings.toml",
	} {
		if !strings.Contains(archive, input) {
			t.Errorf("the web archive does not depend on %s:\n%s", input, archive)
		}
	}
	if !strings.Contains(archive, "web/content.idx-debug: archive_content") {
		t.Errorf("the web archive does not output the debug index:\n%s", archive)
	}

	// NOTE: Updating a tool rebuilds the archives that it produces. The frames
	// and dictionaries depend on zstd, so the archive does not need it.
	if !slices.Contains(implicitInputs(archive), archiveContent) {
		t.Errorf("the web archive does not depend on archive-content:\n%s", archive)
	}

	// NOTE: Desktop archives store files as-is, so they need no frames or zstd.
	if strings.Contains(content, "linux/zstd/files") {
		t.Errorf("the desktop archive compresses files:\n%s", content)
	}
	desktop := implicitInputs(findBuild(t, content, "shipping/linux/content.idx"))
	if !slices.Contains(desktop, archiveContent) || slices.Contains(desktop, zstd) {
		t.Errorf("unexpected inputs of the desktop archive: %v", desktop)
	}
}

func TestRunDictionariesDifferingInCase(t *testing.T) {
	env := newTestEnv(t, testZstdSettings+`
[[strategy.zstd.dictionary]]
name = "Text"
max_size = 4096
paths = ["fonts/*.ttf"]
`, testRecipe)
	content := filepath.ToSlash(readNinjaBuild(t, env))

	// NOTE: Dictionary files are named by their IDs, so names that differ only
	// in case do not collide on case-insensitive file systems.
	for _, tc := range []struct {
		frame      string
		dictionary string
	}{
		{"web/zstd/files/a.json.zst", "1.dict"},
		{"web/zstd/files/fonts/c.ttf.zst", "2.dict"},
	} {
		dictionary := escapeTestPath(filepath.Join(env.IntDir, "web", "zstd", "dictionaries", tc.dictionary))
		findBuild(t, content, "web/zstd/dictionaries/"+tc.dictionary)
		if !slices.Contains(implicitInputs(findBuild(t, content, tc.frame)), dictionary) {
			t.Errorf("%s does not depend on %s:\n%s", tc.frame, tc.dictionary, content)
		}
	}
}

// escapeTestPath escapes a path as the Ninja generator does.
func escapeTestPath(path string) string {
	return strings.ReplaceAll(filepath.ToSlash(path), ":", "$:")
}
