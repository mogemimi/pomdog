// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"slices"
	"strings"
	"testing"
)

func newTestEntry(path string, start, size uint32, contentHash uint64) *entryInfo {
	return &entryInfo{
		Path:             path,
		StartOffset:      start,
		UncompressedSize: 100,
		CompressedSize:   size,
		ContentHash:      &contentHash,
	}
}

func TestStoredBytes(t *testing.T) {
	entries := []*entryInfo{
		newTestEntry("a", 0, 10, 1),
		newTestEntry("b", 0, 10, 1),
		newTestEntry("c", 10, 5, 2),
		newTestEntry("d", 30, 0, 3),
		newTestEntry("e", 12, 6, 4),
	}

	// NOTE: a and b share bytes 0-9, and e overlaps c in bytes 12-14.
	if got := storedBytes(entries); got != 18 {
		t.Errorf("storedBytes() = %d, want 18", got)
	}
}

func TestFindSharedData(t *testing.T) {
	entries := []*entryInfo{
		newTestEntry("x", 20, 4, 5),
		newTestEntry("a", 0, 10, 1),
		newTestEntry("b", 0, 10, 1),
		newTestEntry("c", 10, 5, 2),
		newTestEntry("y", 20, 4, 6),
		newTestEntry("empty1", 30, 0, 3),
		newTestEntry("empty2", 30, 0, 3),
	}

	shared := findSharedData(entries)
	if len(shared) != 2 {
		t.Fatalf("got %d groups, want 2: %+v", len(shared), shared)
	}
	if s := shared[0]; s.StartOffset != 0 || s.Size != 10 || !slices.Equal(s.Paths, []string{"a", "b"}) {
		t.Errorf("unexpected first group: %+v", s)
	}
	if s := shared[1]; s.StartOffset != 20 || s.Size != 4 || !slices.Equal(s.Paths, []string{"x", "y"}) {
		t.Errorf("unexpected second group: %+v", s)
	}
}

func TestFindDuplicates(t *testing.T) {
	entries := []*entryInfo{
		newTestEntry("a", 0, 10, 1),
		newTestEntry("b", 0, 10, 1),
		newTestEntry("c", 10, 100, 1),
		newTestEntry("d", 110, 8, 2),
		newTestEntry("e", 110, 8, 2),
	}

	// NOTE: a, b, and c have the same content, but c is stored in another place.
	duplicates := findDuplicates(entries)
	if len(duplicates) != 1 {
		t.Fatalf("got %d groups, want 1: %+v", len(duplicates), duplicates)
	}
	if d := duplicates[0]; d.ContentHash != 1 || !slices.Equal(d.Paths, []string{"a", "b", "c"}) {
		t.Errorf("unexpected group: %+v", d)
	}
}

// newDictionaryEntry returns an entry for a file that requests dictionary.
// A stored entry uses dictionaries[0].
func newDictionaryEntry(path string, size, requestedSize uint32, dictionary string, stored bool) *entryInfo {
	e := &entryInfo{
		Path:                    path,
		UncompressedSize:        size,
		CompressedSize:          size,
		Compression:             "none",
		RequestedCompression:    "zstd_dictionary",
		RequestedCompressedSize: &requestedSize,
		RequestedDictionary:     dictionary,
	}
	if stored {
		index := uint32(0)
		e.CompressedSize = requestedSize
		e.Compression = "zstd_dictionary"
		e.DictionaryIndex = &index
		e.Dictionary = dictionary
	}
	return e
}

func TestDiagnoseDictionaries(t *testing.T) {
	for _, tc := range []struct {
		name         string
		entries      []*entryInfo
		dictionaries []*dictionaryInfo
		want         []string
	}{
		{
			name: "enough training data",
			entries: []*entryInfo{
				newDictionaryEntry("a.json", 6000, 1000, "text", true),
				newDictionaryEntry("b.json", 6000, 1000, "text", true),
			},
			dictionaries: []*dictionaryInfo{{Name: "text", Size: 1024, SavedBytes: 10000}},
			want:         []string{},
		},
		{
			name: "small training data",
			entries: []*entryInfo{
				newDictionaryEntry("a.json", 3000, 500, "text", true),
				newDictionaryEntry("b.json", 3000, 500, "text", true),
			},
			dictionaries: []*dictionaryInfo{{Name: "text", Size: 1024, SavedBytes: 5000}},
			want:         []string{"small_training_data"},
		},
		{
			// NOTE: zstd reads at most 128 KiB from each file.
			name: "large file",
			entries: []*entryInfo{
				newDictionaryEntry("a.bin", 1024*1024, 1000, "text", true),
			},
			dictionaries: []*dictionaryInfo{{Name: "text", Size: 16 * 1024, SavedBytes: 1024 * 1024}},
			want:         []string{"small_training_data"},
		},
		{
			name: "dictionary larger than savings",
			entries: []*entryInfo{
				newDictionaryEntry("a.json", 20000, 19800, "text", true),
				newDictionaryEntry("b.json", 20000, 19800, "text", true),
			},
			dictionaries: []*dictionaryInfo{{Name: "text", Size: 1024, SavedBytes: 400}},
			want:         []string{"dictionary_larger_than_savings"},
		},
		{
			name: "unused dictionary",
			entries: []*entryInfo{
				newDictionaryEntry("a.reflect", 40, 30, "binary", false),
				newDictionaryEntry("b.json", 6000, 1000, "text", true),
				newDictionaryEntry("c.json", 6000, 1000, "text", true),
			},
			dictionaries: []*dictionaryInfo{{Name: "text", Size: 1024, SavedBytes: 10000}},
			want:         []string{"unused_dictionary"},
		},
	} {
		t.Run(tc.name, func(t *testing.T) {
			diagnostics := diagnoseDictionaries(tc.entries, tc.dictionaries)
			kinds := []string{}
			for _, d := range diagnostics {
				kinds = append(kinds, d.Kind)
			}
			if !slices.Equal(kinds, tc.want) {
				t.Errorf("diagnostics = %v, want %v", kinds, tc.want)
			}
		})
	}
}

func TestWriteTable(t *testing.T) {
	var sb strings.Builder
	writeTable(&sb, "  ",
		[]string{"PATH", "SIZE", "METHOD"},
		[]bool{false, true, false},
		[][]string{
			{"a.json", "12", "zstd"},
			{"shaders/b.glsl", "3456", "none"},
		})

	want := strings.Join([]string{
		"  PATH            SIZE  METHOD",
		"  --------------  ----  ------",
		"  a.json            12  zstd",
		"  shaders/b.glsl  3456  none",
		"",
	}, "\n")
	if sb.String() != want {
		t.Errorf("writeTable() wrote\n%s\nwant\n%s", sb.String(), want)
	}
}
