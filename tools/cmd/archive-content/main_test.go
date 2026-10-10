// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/binary"
	"os"
	"path/filepath"
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	archives "github.com/mogemimi/pomdog/tools/pkg/archives"
	zstdframe "github.com/mogemimi/pomdog/tools/pkg/zstdframe"
)

const testSettings = `[[strategy]]
target_platforms = ["windows", "macos", "linux"]
compression = "none"

[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 64
max_ratio_percent = 90

[[strategy.zstd.dictionary]]
name = "unused"
max_size = 1024
paths = ["**/*.xml"]

[[strategy.zstd.dictionary]]
name = "text"
max_size = 1024
paths = ["text/*.json"]
`

const testRecipe = `[[group]]
paths = [
    "bin/compressible.bin",
    "bin/copy.bin",
    "bin/empty.bin",
    "bin/incompressible.bin",
    "text/a.json",
    "text/b.json",
    "text/same_as_a.json",
    "text/small.json",
]

[[group]]
compression = "none"
paths = [
    "raw/a.json",
    "raw/same_as_a.json",
]
`

// testFile is a content file and the zstd frame size that the test pretends
// zstd produced for it. A frame size of zero means that no frame is written.
type testFile struct {
	path      string
	content   []byte
	frameSize int
}

func testFiles() []testFile {
	compressible := bytes.Repeat([]byte("pomdog "), 200)
	jsonA := []byte(strings.Repeat(`{"name": "a"}`, 40))
	return []testFile{
		{"bin/compressible.bin", compressible, 100},
		{"bin/copy.bin", compressible, 100},
		{"bin/empty.bin", nil, 9},
		{"bin/incompressible.bin", bytes.Repeat([]byte{0x5a}, 1000), 1013},
		{"text/a.json", jsonA, 120},
		{"text/b.json", []byte(strings.Repeat(`{"name": "b"}`, 40)), 121},
		{"text/same_as_a.json", jsonA, 120},
		{"text/small.json", []byte(`{"name": "small"}`), 12},
		{"raw/a.json", jsonA, 0},
		{"raw/same_as_a.json", jsonA, 0},
	}
}

// writeTestFile writes content to dir/name and returns the path.
func writeTestFile(t *testing.T, dir, name string, content []byte) string {
	t.Helper()
	path := filepath.Join(dir, filepath.FromSlash(name))
	if err := os.MkdirAll(filepath.Dir(path), os.ModePerm); err != nil {
		t.Fatalf("os.MkdirAll() failed: %v", err)
	}
	if err := os.WriteFile(path, content, 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	return path
}

// fakeZstdFrame returns frameSize bytes that start with the header of a zstd
// frame for content. archive-content reads only the header of a frame.
func fakeZstdFrame(content []byte, frameSize int, dictionaryID uint32) []byte {
	frame := binary.LittleEndian.AppendUint32(nil, zstdframe.FrameMagic)
	if dictionaryID == 0 {
		// NOTE: Single segment, a 4-byte content size, and a checksum.
		frame = append(frame, 0xa4)
	} else {
		frame = append(frame, 0xa5, byte(dictionaryID))
	}
	frame = binary.LittleEndian.AppendUint32(frame, uint32(len(content)))
	for len(frame) < frameSize {
		frame = append(frame, byte(len(content)+len(frame)))
	}
	return frame
}

// fakeZstdDictionary returns a dictionary with the header of the zstd dictionary format.
func fakeZstdDictionary(id uint32) []byte {
	dictionary := binary.LittleEndian.AppendUint32(nil, zstdframe.DictionaryMagic)
	dictionary = binary.LittleEndian.AppendUint32(dictionary, id)
	return append(dictionary, []byte("dictionary content")...)
}

// testArchive holds the outputs of archive-content.
type testArchive struct {
	archive *schemas.ArchiveT
	debug   *schemas.ArchiveDebugT
	pak     []byte
	entries map[string]*schemas.ArchiveEntryT
	keys    map[string]*schemas.ArchiveKeyT
	debugs  map[string]*schemas.ArchiveKeyDebugT
}

// stored returns the bytes that the archive stores for path.
func (a *testArchive) stored(path string) []byte {
	e := a.entries[path]
	return a.pak[e.StartOffset : e.StartOffset+e.CompressedSize]
}

// testContent writes the content files and the zstd frames and dictionaries
// that archive-ninja-gen would build for the web archive.
func testContent(t *testing.T, dir string) (contentDir, zstdDir string) {
	t.Helper()
	contentDir = filepath.Join(dir, "content")
	zstdDir = filepath.Join(dir, "zstd")
	for _, f := range testFiles() {
		writeTestFile(t, contentDir, f.path, f.content)
		if f.frameSize > 0 {
			dictionaryID := uint32(0)
			if strings.HasPrefix(f.path, "text/") {
				dictionaryID = 2
			}
			writeTestFile(t, zstdDir, filepath.Join("files", f.path+".zst"), fakeZstdFrame(f.content, f.frameSize, dictionaryID))
		}
	}
	writeTestFile(t, zstdDir, "dictionaries/2.dict", fakeZstdDictionary(2))
	return contentDir, zstdDir
}

// buildTestArchive packs the files of recipeContent for platform and reads the outputs.
func buildTestArchive(t *testing.T, env *Env, settingsContent, recipeContent string) (*testArchive, error) {
	t.Helper()
	dir := t.TempDir()

	env.OutIndexFile = filepath.Join(dir, "content.idx")
	env.OutBinaryFile = filepath.Join(dir, "content.pak")
	env.OutDebugFile = filepath.Join(dir, "content.idx-debug")
	env.SettingsFile = writeTestFile(t, dir, "archive_settings.toml", []byte(settingsContent))

	settings := archives.ArchiveSettings{}
	if err := settings.ReadFile(env.SettingsFile); err != nil {
		t.Fatalf("settings.ReadFile() failed: %v", err)
	}
	recipe := archives.ArchiveRecipe{}
	if err := recipe.ReadFile(writeTestFile(t, dir, "archive.toml", []byte(recipeContent))); err != nil {
		t.Fatalf("recipe.ReadFile() failed: %v", err)
	}

	if err := run(env, &settings, &recipe); err != nil {
		return nil, err
	}

	idxBuf, err := os.ReadFile(env.OutIndexFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	debugBuf, err := os.ReadFile(env.OutDebugFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	pak, err := os.ReadFile(env.OutBinaryFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}

	result := &testArchive{
		archive: schemas.GetRootAsArchive(idxBuf, 0).UnPack(),
		debug:   schemas.GetRootAsArchiveDebug(debugBuf, 0).UnPack(),
		pak:     pak,
		entries: map[string]*schemas.ArchiveEntryT{},
		keys:    map[string]*schemas.ArchiveKeyT{},
		debugs:  map[string]*schemas.ArchiveKeyDebugT{},
	}
	paths := map[uint64]string{}
	for _, dk := range result.debug.Keys {
		paths[dk.Key] = dk.Path
		result.debugs[dk.Path] = dk
	}
	for _, key := range result.archive.Keys {
		result.keys[paths[key.Key]] = key
		result.entries[paths[key.Key]] = result.archive.Entries[key.Index]
	}
	return result, nil
}

func buildWebArchive(t *testing.T) *testArchive {
	t.Helper()
	contentDir, zstdDir := testContent(t, t.TempDir())
	result, err := buildTestArchive(t, &Env{
		ContentDir:      contentDir,
		ZstdDir:         zstdDir,
		TargetPlatforms: stringSlice{archives.PlatformWeb},
	}, testSettings, testRecipe)
	if err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	return result
}

func TestZstdFallback(t *testing.T) {
	result := buildWebArchive(t)

	for _, tc := range []struct {
		path      string
		requested schemas.ArchiveCompression
		stored    schemas.ArchiveCompression
	}{
		{"bin/compressible.bin", schemas.ArchiveCompressionZstd, schemas.ArchiveCompressionZstd},
		{"bin/incompressible.bin", schemas.ArchiveCompressionZstd, schemas.ArchiveCompressionNone},
		{"bin/empty.bin", schemas.ArchiveCompressionZstd, schemas.ArchiveCompressionNone},
		{"text/a.json", schemas.ArchiveCompressionZstdDictionary, schemas.ArchiveCompressionZstdDictionary},
		{"text/small.json", schemas.ArchiveCompressionZstdDictionary, schemas.ArchiveCompressionNone},
		{"raw/a.json", schemas.ArchiveCompressionNone, schemas.ArchiveCompressionNone},
	} {
		entry := result.entries[tc.path]
		debug := result.debugs[tc.path]
		if entry == nil || debug == nil {
			t.Fatalf("%s: missing from the archive", tc.path)
		}
		if entry.Compression != tc.stored {
			t.Errorf("%s: stored as %v, want %v", tc.path, entry.Compression, tc.stored)
		}
		if debug.RequestedCompression != tc.requested {
			t.Errorf("%s: requested %v, want %v", tc.path, debug.RequestedCompression, tc.requested)
		}
		if entry.Compression == schemas.ArchiveCompressionNone && entry.CompressedSize != entry.UncompressedSize {
			t.Errorf("%s: stores %d bytes for a %d-byte file", tc.path, entry.CompressedSize, entry.UncompressedSize)
		}
		if entry.Compression == debug.RequestedCompression && debug.RequestedCompressedSize != entry.CompressedSize {
			t.Errorf("%s: requested compressed size %d differs from the stored %d bytes", tc.path, debug.RequestedCompressedSize, entry.CompressedSize)
		}
	}

	// NOTE: A fallback keeps the size that zstd produced for diagnostics.
	if size := result.debugs["bin/incompressible.bin"].RequestedCompressedSize; size != 1013 {
		t.Errorf("bin/incompressible.bin: requested compressed size %d, want 1013", size)
	}
	if name := result.debugs["text/small.json"].RequestedDictionary; name != "text" {
		t.Errorf("text/small.json: requested dictionary %q, want \"text\"", name)
	}
	if name := result.debugs["bin/compressible.bin"].RequestedDictionary; name != "" {
		t.Errorf("bin/compressible.bin: requested dictionary %q, want none", name)
	}
}

func TestDictionaries(t *testing.T) {
	result := buildWebArchive(t)

	// NOTE: The index holds only the dictionaries that stored entries use.
	if len(result.archive.Dictionaries) != 1 || len(result.debug.Dictionaries) != 1 {
		t.Fatalf("got %d dictionaries and %d debug dictionaries, want 1", len(result.archive.Dictionaries), len(result.debug.Dictionaries))
	}
	if !bytes.Equal(result.archive.Dictionaries[0].Data, fakeZstdDictionary(2)) {
		t.Error("the dictionary data differs from the dictionary file")
	}
	if name := result.debug.Dictionaries[0].Name; name != "text" {
		t.Errorf("debug dictionary name %q, want \"text\"", name)
	}

	for _, path := range []string{"text/a.json", "text/b.json"} {
		if entry := result.entries[path]; entry.DictionaryIndex != 0 {
			t.Errorf("%s: dictionary index %d, want 0", path, entry.DictionaryIndex)
		}
	}
}

func TestSharedData(t *testing.T) {
	result := buildWebArchive(t)

	// NOTE: Keys of files stored in the same way share one entry.
	for _, pair := range [][2]string{
		{"bin/compressible.bin", "bin/copy.bin"},
		{"text/a.json", "text/same_as_a.json"},
		{"raw/a.json", "raw/same_as_a.json"},
	} {
		if a, b := result.keys[pair[0]].Index, result.keys[pair[1]].Index; a != b {
			t.Errorf("%s and %s use entries %d and %d, want one entry", pair[0], pair[1], a, b)
		}
	}

	// NOTE: The same content stored in different ways uses different entries.
	if a, b := result.keys["text/a.json"].Index, result.keys["raw/a.json"].Index; a == b {
		t.Errorf("text/a.json and raw/a.json share entry %d", a)
	}

	// NOTE: Identical stored bytes are stored once, even for different entries.
	empty := result.entries["bin/empty.bin"]
	if empty.CompressedSize != 0 {
		t.Errorf("bin/empty.bin: stores %d bytes, want 0", empty.CompressedSize)
	}

	stored := map[string]bool{}
	total := 0
	for path := range result.entries {
		s := result.stored(path)
		if !stored[string(s)] {
			stored[string(s)] = true
			total += len(s)
		}
	}
	if total != len(result.pak) {
		t.Errorf("the pak file has %d bytes, want %d bytes of unique stored data", len(result.pak), total)
	}

	for _, f := range testFiles() {
		entry := result.entries[f.path]
		if entry.Compression == schemas.ArchiveCompressionNone && !bytes.Equal(result.stored(f.path), f.content) {
			t.Errorf("%s: stored bytes differ from the content", f.path)
		}
	}
}

func TestDeterministicOutput(t *testing.T) {
	a := buildWebArchive(t)
	b := buildWebArchive(t)
	if !bytes.Equal(a.pak, b.pak) {
		t.Error("two builds produce different pak files")
	}
}

func TestStrategyWithoutCompression(t *testing.T) {
	contentDir, _ := testContent(t, t.TempDir())
	result, err := buildTestArchive(t, &Env{
		ContentDir:      contentDir,
		TargetPlatforms: stringSlice{archives.PlatformWindows},
	}, testSettings, testRecipe)
	if err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	for path, entry := range result.entries {
		if entry.Compression != schemas.ArchiveCompressionNone {
			t.Errorf("%s: stored as %v, want None", path, entry.Compression)
		}
		if requested := result.debugs[path].RequestedCompression; requested != schemas.ArchiveCompressionNone {
			t.Errorf("%s: requested %v, want None", path, requested)
		}
	}
	if len(result.archive.Dictionaries) != 0 {
		t.Errorf("got %d dictionaries, want none", len(result.archive.Dictionaries))
	}
}

func TestInvalidZstdInputs(t *testing.T) {
	for _, tc := range []struct {
		name   string
		modify func(t *testing.T, zstdDir string)
		env    func(env *Env)
		want   string
	}{
		{"missing zstddir", nil, func(env *Env) { env.ZstdDir = "" }, "--zstddir is required to pack bin/compressible.bin"},
		{"missing frame", func(t *testing.T, zstdDir string) {
			if err := os.Remove(filepath.Join(zstdDir, "files", "bin", "copy.bin.zst")); err != nil {
				t.Fatalf("os.Remove() failed: %v", err)
			}
		}, nil, "copy.bin.zst"},
		{"missing dictionary", func(t *testing.T, zstdDir string) {
			if err := os.Remove(filepath.Join(zstdDir, "dictionaries", "2.dict")); err != nil {
				t.Fatalf("os.Remove() failed: %v", err)
			}
		}, nil, "2.dict"},
		{"stale frame", func(t *testing.T, zstdDir string) {
			writeTestFile(t, zstdDir, "files/bin/copy.bin.zst", fakeZstdFrame([]byte("stale"), 100, 0))
		}, nil, "the frame holds 5 bytes, but bin/copy.bin has 1400 bytes"},
		{"frame without dictionary", func(t *testing.T, zstdDir string) {
			writeTestFile(t, zstdDir, "files/text/b.json.zst", fakeZstdFrame([]byte(strings.Repeat(`{"name": "b"}`, 40)), 121, 0))
		}, nil, "the frame records dictionary ID 0, want 2"},
		{"wrong dictionary ID", func(t *testing.T, zstdDir string) {
			writeTestFile(t, zstdDir, "dictionaries/2.dict", fakeZstdDictionary(1))
		}, nil, "dictionary ID is 1, want 2"},
	} {
		t.Run(tc.name, func(t *testing.T) {
			contentDir, zstdDir := testContent(t, t.TempDir())
			if tc.modify != nil {
				tc.modify(t, zstdDir)
			}
			env := &Env{
				ContentDir:      contentDir,
				ZstdDir:         zstdDir,
				TargetPlatforms: stringSlice{archives.PlatformWeb},
			}
			if tc.env != nil {
				tc.env(env)
			}
			_, err := buildTestArchive(t, env, testSettings, testRecipe)
			if err == nil {
				t.Fatal("run() succeeded, want an error")
			}
			if !strings.Contains(filepath.ToSlash(err.Error()), tc.want) {
				t.Errorf("error %q does not contain %q", err, tc.want)
			}
		})
	}
}

func TestGroupZstdWithoutSettings(t *testing.T) {
	recipe := `[[group]]
compression = "zstd"
paths = [
    "text/a.json",
]
`
	settings := `[[strategy]]
target_platforms = ["windows"]
compression = "none"
`
	contentDir, zstdDir := testContent(t, t.TempDir())
	_, err := buildTestArchive(t, &Env{
		ContentDir:      contentDir,
		ZstdDir:         zstdDir,
		TargetPlatforms: stringSlice{archives.PlatformWindows},
	}, settings, recipe)
	if err == nil {
		t.Fatal("run() succeeded, want an error for zstd without [strategy.zstd] settings")
	}
	if !strings.Contains(err.Error(), "[strategy.zstd]") {
		t.Errorf("unexpected error: %v", err)
	}
}
