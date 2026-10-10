// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"os"
	"sort"
	"strings"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	zstdframe "github.com/mogemimi/pomdog/tools/pkg/zstdframe"
)

func main() {
	env := Env{}
	flag.StringVar(&env.IndexFile, "idx", "", "input index file (*.idx)")
	flag.StringVar(&env.PakFile, "pak", "", "input binary archive file (*.pak); checks entry ranges and zstd frame headers")
	flag.StringVar(&env.DebugFile, "debug", "", "input debug index file (*.idx-debug)")
	flag.StringVar(&env.OutFile, "o", "", "output file (default: stdout)")
	flag.BoolVar(&env.JSON, "json", false, "output in JSON format")
	flag.Parse()

	if env.IndexFile == "" {
		log.Fatal("-idx is required")
	}

	if err := run(&env); err != nil {
		log.Fatal(err)
	}
}

type Env struct {
	IndexFile string
	PakFile   string
	DebugFile string
	OutFile   string
	JSON      bool
}

// entryInfo holds the merged information for a single archive key and its entry.
type entryInfo struct {
	Path             string  `json:"path,omitempty"`
	Key              uint64  `json:"key"`
	Index            uint32  `json:"index"`
	StartOffset      uint32  `json:"start_offset"`
	UncompressedSize uint32  `json:"uncompressed_size"`
	CompressedSize   uint32  `json:"compressed_size"`
	Compression      string  `json:"compression"`
	DictionaryIndex  *uint32 `json:"dictionary_index,omitempty"`
	Dictionary       string  `json:"dictionary,omitempty"`
	ContentHash      *uint64 `json:"content_hash,omitempty"`
	WindowSize       *uint64 `json:"window_size,omitempty"`

	// NOTE: The requested method, its size, and its dictionary come from the debug index.
	RequestedCompression    string  `json:"requested_compression,omitempty"`
	RequestedCompressedSize *uint32 `json:"requested_compressed_size,omitempty"`
	RequestedDictionary     string  `json:"requested_dictionary,omitempty"`

	compression schemas.ArchiveCompression
}

// name returns the path of the entry, or its key when the path is unknown.
func (e *entryInfo) name() string {
	if e.Path != "" {
		return e.Path
	}
	return fmt.Sprintf("0x%016x", e.Key)
}

// method returns the compression method with the dictionary that the entry uses.
func (e *entryInfo) method() string {
	if e.DictionaryIndex == nil {
		return e.Compression
	}
	if e.Dictionary != "" {
		return fmt.Sprintf("%s(%s)", e.Compression, e.Dictionary)
	}
	return fmt.Sprintf("%s(#%d)", e.Compression, *e.DictionaryIndex)
}

// requestedMethod returns the requested method with the dictionary that it uses.
func (e *entryInfo) requestedMethod() string {
	if e.RequestedDictionary != "" {
		return fmt.Sprintf("%s(%s)", e.RequestedCompression, e.RequestedDictionary)
	}
	return e.RequestedCompression
}

// fellBack reports whether archive-content stored the entry as-is because the
// requested method did not save enough.
func (e *entryInfo) fellBack() bool {
	return e.RequestedCompression != "" && e.RequestedCompression != e.Compression
}

// dictionaryInfo describes a dictionary in the archive index.
type dictionaryInfo struct {
	Index uint32 `json:"index"`
	Name  string `json:"name,omitempty"`
	ID    uint32 `json:"id"`
	Size  int    `json:"size"`
	Files int    `json:"files"`

	// SavedBytes is the number of bytes that the entries using the dictionary
	// save compared with storing their files as-is.
	SavedBytes uint64 `json:"saved_bytes"`
}

// diagnosticInfo describes archive settings that may be improved.
type diagnosticInfo struct {
	Kind       string `json:"kind"`
	Dictionary string `json:"dictionary"`
	Message    string `json:"message"`
}

// sharedDataInfo lists files whose entries refer to the same stored bytes.
type sharedDataInfo struct {
	StartOffset uint32   `json:"start_offset"`
	Size        uint32   `json:"size"`
	Paths       []string `json:"paths"`
}

// duplicateInfo lists files with identical content that the archive stores
// more than once.
type duplicateInfo struct {
	ContentHash uint64   `json:"content_hash"`
	Size        uint32   `json:"size"`
	Paths       []string `json:"paths"`
}

// archiveInfo is the top-level JSON output structure.
type archiveInfo struct {
	PakFile                string `json:"pak_file,omitempty"`
	TotalFiles             int    `json:"total_files"`
	TotalUncompressedBytes uint64 `json:"total_uncompressed_bytes"`

	// NOTE: TotalCompressedBytes counts the stored bytes of every file, and
	// TotalStoredBytes counts the bytes that files share only once.
	TotalCompressedBytes uint64 `json:"total_compressed_bytes"`
	TotalStoredBytes     uint64 `json:"total_stored_bytes"`
	TotalDictionaryBytes uint64 `json:"total_dictionary_bytes"`

	MaxWindowSize *uint64           `json:"max_window_size,omitempty"`
	Seed          *uint64           `json:"seed,omitempty"`
	Dictionaries  []*dictionaryInfo `json:"dictionaries,omitempty"`
	Entries       []*entryInfo      `json:"entries"`
	SharedData    []*sharedDataInfo `json:"shared_data,omitempty"`
	Duplicates    []*duplicateInfo  `json:"duplicates,omitempty"`
	Diagnostics   []*diagnosticInfo `json:"diagnostics,omitempty"`
}

// compressionName returns the name that the archive tools use for c.
func compressionName(c schemas.ArchiveCompression) (string, error) {
	switch c {
	case schemas.ArchiveCompressionNone:
		return "none", nil
	case schemas.ArchiveCompressionZstd:
		return "zstd", nil
	case schemas.ArchiveCompressionZstdDictionary:
		return "zstd_dictionary", nil
	}
	return "", fmt.Errorf("unknown compression %d", c)
}

func run(env *Env) error {
	// NOTE: Read the index file (.idx).
	idxBuf, err := os.ReadFile(env.IndexFile)
	if err != nil {
		return fmt.Errorf("failed to read index file: %w", err)
	}
	archive := schemas.GetRootAsArchive(idxBuf, 0)
	archiveT := archive.UnPack()

	dictionaries := make([]*dictionaryInfo, 0, len(archiveT.Dictionaries))
	for i, d := range archiveT.Dictionaries {
		id, err := zstdframe.DictionaryID(d.Data)
		if err != nil {
			return fmt.Errorf("dictionary %d: %w", i, err)
		}
		dictionaries = append(dictionaries, &dictionaryInfo{Index: uint32(i), ID: id, Size: len(d.Data)})
	}

	// NOTE: Build a map from key hash to debug entry (if debug file is provided).
	debugKeys := map[uint64]*schemas.ArchiveKeyDebugT{}
	var seed *uint64
	if env.DebugFile != "" {
		debugBuf, err := os.ReadFile(env.DebugFile)
		if err != nil {
			return fmt.Errorf("failed to read debug file: %w", err)
		}
		debugArchive := schemas.GetRootAsArchiveDebug(debugBuf, 0)
		debugT := debugArchive.UnPack()

		s := debugT.Seed
		seed = &s

		for _, dk := range debugT.Keys {
			debugKeys[dk.Key] = dk
		}

		if len(debugT.Dictionaries) != len(dictionaries) {
			return fmt.Errorf("the debug index names %d dictionaries, but the index has %d", len(debugT.Dictionaries), len(dictionaries))
		}
		for i, d := range debugT.Dictionaries {
			dictionaries[i].Name = d.Name
		}
	}

	// NOTE: Merge keys and entries into a unified list.
	entries := make([]*entryInfo, 0, len(archiveT.Keys))
	countedEntries := map[uint32]struct{}{}
	for _, key := range archiveT.Keys {
		idx := key.Index
		if int(idx) >= len(archiveT.Entries) {
			return fmt.Errorf("key 0x%016x references out-of-range entry index %d", key.Key, idx)
		}
		e := archiveT.Entries[idx]
		info := &entryInfo{
			Key:              key.Key,
			Index:            idx,
			StartOffset:      e.StartOffset,
			UncompressedSize: e.UncompressedSize,
			CompressedSize:   e.CompressedSize,
			compression:      e.Compression,
		}
		if dk, ok := debugKeys[key.Key]; ok {
			contentHash := dk.ContentHash
			requestedCompressedSize := dk.RequestedCompressedSize
			info.Path = dk.Path
			info.ContentHash = &contentHash
			info.RequestedCompressedSize = &requestedCompressedSize
			info.RequestedDictionary = dk.RequestedDictionary
			if info.RequestedCompression, err = compressionName(dk.RequestedCompression); err != nil {
				return fmt.Errorf("debug entry %s: %w", info.name(), err)
			}
		}
		if info.Compression, err = compressionName(e.Compression); err != nil {
			return fmt.Errorf("entry %s: %w", info.name(), err)
		}

		switch e.Compression {
		case schemas.ArchiveCompressionNone:
			if e.CompressedSize != e.UncompressedSize {
				return fmt.Errorf("uncompressed entry %s stores %d bytes for a %d-byte file", info.name(), e.CompressedSize, e.UncompressedSize)
			}
		case schemas.ArchiveCompressionZstdDictionary:
			if int(e.DictionaryIndex) >= len(dictionaries) {
				return fmt.Errorf("entry %s references out-of-range dictionary index %d", info.name(), e.DictionaryIndex)
			}
			dictionaryIndex := uint32(e.DictionaryIndex)
			info.DictionaryIndex = &dictionaryIndex
			info.Dictionary = dictionaries[dictionaryIndex].Name
			dictionaries[dictionaryIndex].Files++

			// NOTE: Keys that share an entry save its bytes only once.
			if _, ok := countedEntries[idx]; !ok {
				countedEntries[idx] = struct{}{}
				if e.UncompressedSize > e.CompressedSize {
					dictionaries[dictionaryIndex].SavedBytes += uint64(e.UncompressedSize - e.CompressedSize)
				}
			}
		}
		entries = append(entries, info)
	}

	// NOTE: Sort by key, then stably by path, so that entries without a path or
	// with the same path stay in key order.
	sort.SliceStable(entries, func(i, j int) bool {
		return entries[i].Key < entries[j].Key
	})
	sort.SliceStable(entries, func(i, j int) bool {
		return entries[i].Path < entries[j].Path
	})

	info := &archiveInfo{
		PakFile:      env.PakFile,
		TotalFiles:   len(entries),
		Seed:         seed,
		Dictionaries: dictionaries,
		Entries:      entries,
	}

	for _, e := range entries {
		info.TotalUncompressedBytes += uint64(e.UncompressedSize)
		info.TotalCompressedBytes += uint64(e.CompressedSize)
	}
	info.TotalStoredBytes = storedBytes(entries)
	for _, d := range dictionaries {
		info.TotalDictionaryBytes += uint64(d.Size)
	}
	info.SharedData = findSharedData(entries)

	if env.PakFile != "" {
		if err := readPakFrames(env.PakFile, entries, dictionaries); err != nil {
			return err
		}
		for _, e := range entries {
			if e.WindowSize != nil && (info.MaxWindowSize == nil || *info.MaxWindowSize < *e.WindowSize) {
				info.MaxWindowSize = e.WindowSize
			}
		}
	}

	if env.DebugFile != "" {
		info.Duplicates = findDuplicates(entries)
		info.Diagnostics = diagnoseDictionaries(entries, dictionaries)
	}

	// NOTE: Format output.
	var output string
	if env.JSON {
		buf, err := json.MarshalIndent(info, "", "  ")
		if err != nil {
			return fmt.Errorf("json.MarshalIndent() failed: %w", err)
		}
		output = string(buf) + "\n"
	} else {
		output = formatText(info)
	}

	// NOTE: Write output.
	if env.OutFile != "" {
		if err := os.WriteFile(env.OutFile, []byte(output), 0o644); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	} else {
		fmt.Print(output)
	}

	return nil
}

// storedBytes returns the number of .pak bytes that the entries refer to,
// counting bytes that several entries share only once.
func storedBytes(entries []*entryInfo) uint64 {
	type byteRange struct{ start, end uint64 }
	ranges := make([]byteRange, 0, len(entries))
	for _, e := range entries {
		if e.CompressedSize > 0 {
			start := uint64(e.StartOffset)
			ranges = append(ranges, byteRange{start, start + uint64(e.CompressedSize)})
		}
	}
	sort.Slice(ranges, func(i, j int) bool {
		return ranges[i].start < ranges[j].start
	})

	total := uint64(0)
	end := uint64(0)
	for _, r := range ranges {
		start := max(r.start, end)
		if r.end > start {
			total += r.end - start
		}
		end = max(end, r.end)
	}
	return total
}

// findSharedData returns the groups of files whose entries refer to the same
// stored bytes, ordered by offset.
func findSharedData(entries []*entryInfo) []*sharedDataInfo {
	type dataKey struct {
		start uint32
		size  uint32
	}

	groups := map[dataKey]*sharedDataInfo{}
	for _, e := range entries {
		if e.CompressedSize == 0 {
			continue
		}
		k := dataKey{start: e.StartOffset, size: e.CompressedSize}
		group, ok := groups[k]
		if !ok {
			group = &sharedDataInfo{StartOffset: k.start, Size: k.size}
			groups[k] = group
		}
		group.Paths = append(group.Paths, e.name())
	}

	shared := []*sharedDataInfo{}
	for _, group := range groups {
		if len(group.Paths) > 1 {
			shared = append(shared, group)
		}
	}
	sort.Slice(shared, func(i, j int) bool {
		if shared[i].StartOffset != shared[j].StartOffset {
			return shared[i].StartOffset < shared[j].StartOffset
		}
		return shared[i].Size < shared[j].Size
	})
	return shared
}

// readPakFrames checks that every entry lies inside the .pak file and reads
// the frame header of each zstd entry. It records the window size, which is
// the history that a streaming decoder keeps for the entry.
func readPakFrames(pakFile string, entries []*entryInfo, dictionaries []*dictionaryInfo) error {
	f, err := os.Open(pakFile)
	if err != nil {
		return fmt.Errorf("failed to open pak file: %w", err)
	}
	defer f.Close()

	stat, err := f.Stat()
	if err != nil {
		return fmt.Errorf("f.Stat() failed: %w", err)
	}
	pakSize := uint64(stat.Size())

	for _, e := range entries {
		end := uint64(e.StartOffset) + uint64(e.CompressedSize)
		if end > pakSize {
			return fmt.Errorf("entry %s ends at byte %d, beyond the pak size %d", e.name(), end, pakSize)
		}

		if e.compression != schemas.ArchiveCompressionZstd && e.compression != schemas.ArchiveCompressionZstdDictionary {
			continue
		}

		buf := make([]byte, min(uint64(zstdframe.MaxHeaderSize), uint64(e.CompressedSize)))
		if _, err := f.ReadAt(buf, int64(e.StartOffset)); err != nil {
			return fmt.Errorf("failed to read entry %s: %w", e.name(), err)
		}

		header, err := zstdframe.ParseHeader(buf)
		if err != nil {
			return fmt.Errorf("invalid zstd frame header in entry %s: %w", e.name(), err)
		}
		if !header.HasContentSize {
			return fmt.Errorf("entry %s has no frame content size", e.name())
		}
		if header.ContentSize != uint64(e.UncompressedSize) {
			return fmt.Errorf("entry %s has a frame content size of %d bytes, but the index records %d bytes", e.name(), header.ContentSize, e.UncompressedSize)
		}

		// NOTE: A frame that records a dictionary ID decodes only with that dictionary.
		dictionaryID := uint32(0)
		if e.DictionaryIndex != nil {
			dictionaryID = dictionaries[*e.DictionaryIndex].ID
		}
		if header.DictionaryID != 0 && header.DictionaryID != dictionaryID {
			return fmt.Errorf("entry %s has a frame for dictionary ID %d, but its dictionary has ID %d", e.name(), header.DictionaryID, dictionaryID)
		}

		windowSize := header.WindowSize
		e.WindowSize = &windowSize
	}

	return nil
}

// findDuplicates returns the groups of entries with the same content hash and
// size that the archive stores in more than one place, ordered by the first
// path of each group.
func findDuplicates(entries []*entryInfo) []*duplicateInfo {
	type contentKey struct {
		hash uint64
		size uint32
	}
	type dataKey struct {
		start uint32
		size  uint32
	}

	groups := map[contentKey]*duplicateInfo{}
	locations := map[contentKey]map[dataKey]struct{}{}
	order := []contentKey{}
	for _, e := range entries {
		if e.ContentHash == nil {
			continue
		}
		k := contentKey{hash: *e.ContentHash, size: e.UncompressedSize}
		group, ok := groups[k]
		if !ok {
			group = &duplicateInfo{ContentHash: k.hash, Size: k.size}
			groups[k] = group
			locations[k] = map[dataKey]struct{}{}
			order = append(order, k)
		}
		group.Paths = append(group.Paths, e.name())
		locations[k][dataKey{start: e.StartOffset, size: e.CompressedSize}] = struct{}{}
	}

	duplicates := []*duplicateInfo{}
	for _, k := range order {
		if len(locations[k]) > 1 {
			duplicates = append(duplicates, groups[k])
		}
	}
	return duplicates
}

const (
	// minTrainingDataRatio is the size of the training data relative to the
	// dictionary size that zstd recommends at least. With less data, zstd --train
	// warns that the dictionary may be subpar.
	minTrainingDataRatio = 10

	// maxTrainingSampleSize is the size that zstd --train reads from each file.
	maxTrainingSampleSize = 128 * 1024
)

// diagnoseDictionaries reports dictionaries whose settings may be improved. It
// needs the requested dictionaries from the debug index.
func diagnoseDictionaries(entries []*entryInfo, dictionaries []*dictionaryInfo) []*diagnosticInfo {
	// NOTE: The files that request a dictionary are the files it was trained on.
	type trainingSet struct {
		files int
		bytes uint64
	}
	trainingSets := map[string]*trainingSet{}
	names := []string{}
	for _, e := range entries {
		if e.RequestedDictionary == "" {
			continue
		}
		set, ok := trainingSets[e.RequestedDictionary]
		if !ok {
			set = &trainingSet{}
			trainingSets[e.RequestedDictionary] = set
			names = append(names, e.RequestedDictionary)
		}
		set.files++
		set.bytes += min(uint64(e.UncompressedSize), maxTrainingSampleSize)
	}
	sort.Strings(names)

	stored := map[string]*dictionaryInfo{}
	for _, d := range dictionaries {
		stored[d.Name] = d
	}

	diagnostics := []*diagnosticInfo{}
	for _, name := range names {
		set := trainingSets[name]
		d, ok := stored[name]
		if !ok {
			diagnostics = append(diagnostics, &diagnosticInfo{
				Kind:       "unused_dictionary",
				Dictionary: name,
				Message: fmt.Sprintf("The index has no dictionary %q, because none of its %d files saves enough with it. Remove the dictionary or change its paths.",
					name, set.files),
			})
			continue
		}

		if set.bytes < uint64(d.Size)*minTrainingDataRatio {
			diagnostics = append(diagnostics, &diagnosticInfo{
				Kind:       "small_training_data",
				Dictionary: name,
				Message: fmt.Sprintf("Dictionary %q is %s, but its %d files have %s of training data (%.1f times the dictionary), less than the %d times that zstd recommends. A smaller max_size may make the archive smaller.",
					name, formatSize(uint64(d.Size)), set.files, formatSize(set.bytes), float64(set.bytes)/float64(d.Size), minTrainingDataRatio),
			})
		}

		// NOTE: Without the dictionary, every file would be stored in at most its
		// own size, so a dictionary that saves less than its size enlarges the archive.
		if d.SavedBytes < uint64(d.Size) {
			diagnostics = append(diagnostics, &diagnosticInfo{
				Kind:       "dictionary_larger_than_savings",
				Dictionary: name,
				Message: fmt.Sprintf("Dictionary %q takes %s in the index, but its files save only %s compared with storing them as-is. The archive is smaller without it.",
					name, formatSize(uint64(d.Size)), formatSize(d.SavedBytes)),
			})
		}
	}
	return diagnostics
}

// writeTable writes rows as a table with a header line and a separator line,
// indenting each line with indent. Columns whose rightAlign is true are aligned
// to the right.
func writeTable(sb *strings.Builder, indent string, headers []string, rightAlign []bool, rows [][]string) {
	widths := make([]int, len(headers))
	for i, header := range headers {
		widths[i] = len(header)
	}
	for _, row := range rows {
		for i, cell := range row {
			widths[i] = max(widths[i], len(cell))
		}
	}

	writeRow := func(cells []string) {
		sb.WriteString(indent)
		for i, cell := range cells {
			if i > 0 {
				sb.WriteString("  ")
			}
			switch {
			case rightAlign[i]:
				sb.WriteString(fmt.Sprintf("%*s", widths[i], cell))
			case i == len(cells)-1:
				// NOTE: The last column is not padded to avoid trailing spaces.
				sb.WriteString(cell)
			default:
				sb.WriteString(fmt.Sprintf("%-*s", widths[i], cell))
			}
		}
		sb.WriteString("\n")
	}

	writeRow(headers)
	separators := make([]string, len(headers))
	for i := range headers {
		separators[i] = strings.Repeat("-", widths[i])
	}
	writeRow(separators)
	for _, row := range rows {
		writeRow(row)
	}
}

func formatText(info *archiveInfo) string {
	var sb strings.Builder

	sb.WriteString(fmt.Sprintf("Archive: %d files, %s, stored as %s (%s)\n",
		info.TotalFiles,
		formatSize(info.TotalUncompressedBytes),
		formatSize(info.TotalStoredBytes),
		formatRatio(info.TotalStoredBytes, info.TotalUncompressedBytes)))

	// NOTE: Totals for each compression method.
	{
		type methodTotal struct {
			files        int
			uncompressed uint64
			compressed   uint64
		}
		totals := map[string]*methodTotal{}
		methods := []string{}
		for _, e := range info.Entries {
			t, ok := totals[e.Compression]
			if !ok {
				t = &methodTotal{}
				totals[e.Compression] = t
				methods = append(methods, e.Compression)
			}
			t.files++
			t.uncompressed += uint64(e.UncompressedSize)
			t.compressed += uint64(e.CompressedSize)
		}
		sort.Strings(methods)
		for _, m := range methods {
			t := totals[m]
			sb.WriteString(fmt.Sprintf("  %s: %d files, %s, stored as %s (%s)\n",
				m, t.files, formatSize(t.uncompressed), formatSize(t.compressed), formatRatio(t.compressed, t.uncompressed)))
		}
	}

	if len(info.SharedData) == 0 {
		sb.WriteString("Shared stored bytes: none\n")
	} else {
		files := 0
		for _, s := range info.SharedData {
			files += len(s.Paths)
		}
		sb.WriteString(fmt.Sprintf("Shared stored bytes: %d files in %d groups, saving %s\n",
			files, len(info.SharedData), formatSize(info.TotalCompressedBytes-info.TotalStoredBytes)))
	}

	if len(info.Dictionaries) == 0 {
		sb.WriteString("Dictionaries: none\n")
	} else {
		sb.WriteString(fmt.Sprintf("Dictionaries: %d, %s in the index\n", len(info.Dictionaries), formatSize(info.TotalDictionaryBytes)))
		for _, d := range info.Dictionaries {
			name := d.Name
			if name == "" {
				name = "-"
			}
			sb.WriteString(fmt.Sprintf("  #%d %s: ID %d, %s, %d files saving %s\n", d.Index, name, d.ID, formatSize(uint64(d.Size)), d.Files, formatSize(d.SavedBytes)))
		}
	}

	if info.PakFile != "" {
		sb.WriteString(fmt.Sprintf("Pak: %s\n", info.PakFile))
	}
	if info.MaxWindowSize != nil {
		sb.WriteString(fmt.Sprintf("Max zstd window: %s\n", formatSize(*info.MaxWindowSize)))
	}
	if info.Seed != nil {
		sb.WriteString(fmt.Sprintf("Seed: %d (0x%x)\n", *info.Seed, *info.Seed))
	}
	sb.WriteString("\n")

	// NOTE: Column headers.
	hasPath := false
	for _, e := range info.Entries {
		if e.Path != "" {
			hasPath = true
			break
		}
	}

	if hasPath {
		sb.WriteString(fmt.Sprintf("%-70s %18s %10s %10s %10s %7s %s\n", "PATH", "KEY", "OFFSET", "SIZE", "STORED", "RATIO", "METHOD"))
		sb.WriteString(fmt.Sprintf("%s %s %s %s %s %s %s\n",
			strings.Repeat("-", 70),
			strings.Repeat("-", 18),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
			strings.Repeat("-", 7),
			strings.Repeat("-", 6),
		))
		for _, e := range info.Entries {
			sb.WriteString(fmt.Sprintf("%-70s 0x%016x %10d %10d %10d %7s %s\n",
				e.Path, e.Key, e.StartOffset, e.UncompressedSize, e.CompressedSize,
				formatRatio(uint64(e.CompressedSize), uint64(e.UncompressedSize)), e.method()))
		}
	} else {
		sb.WriteString(fmt.Sprintf("%18s %10s %10s %10s %7s %s\n", "KEY", "OFFSET", "SIZE", "STORED", "RATIO", "METHOD"))
		sb.WriteString(fmt.Sprintf("%s %s %s %s %s %s\n",
			strings.Repeat("-", 18),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
			strings.Repeat("-", 7),
			strings.Repeat("-", 6),
		))
		for _, e := range info.Entries {
			sb.WriteString(fmt.Sprintf("0x%016x %10d %10d %10d %7s %s\n",
				e.Key, e.StartOffset, e.UncompressedSize, e.CompressedSize,
				formatRatio(uint64(e.CompressedSize), uint64(e.UncompressedSize)), e.method()))
		}
	}

	// NOTE: Files that share stored bytes, in the order of the .pak file.
	if len(info.SharedData) > 0 {
		sb.WriteString(fmt.Sprintf("\nShared stored bytes: %d groups\n", len(info.SharedData)))
		for _, s := range info.SharedData {
			sb.WriteString(fmt.Sprintf("  offset %d, %s, %d files:\n", s.StartOffset, formatSize(uint64(s.Size)), len(s.Paths)))
			for _, path := range s.Paths {
				sb.WriteString(fmt.Sprintf("    %s\n", path))
			}
		}
	}

	// NOTE: Fallbacks are detectable only with the requested methods from the debug index.
	if info.Seed != nil {
		fallbacks := []*entryInfo{}
		for _, e := range info.Entries {
			if e.fellBack() {
				fallbacks = append(fallbacks, e)
			}
		}

		sb.WriteString("\n")
		if len(fallbacks) == 0 {
			sb.WriteString("Stored as-is instead of the requested method: none\n")
		} else {
			sb.WriteString(fmt.Sprintf("Stored as-is instead of the requested method: %d files\n", len(fallbacks)))
			rows := make([][]string, 0, len(fallbacks))
			for _, e := range fallbacks {
				rows = append(rows, []string{
					e.name(),
					fmt.Sprintf("%d", e.UncompressedSize),
					fmt.Sprintf("%d", *e.RequestedCompressedSize),
					formatRatio(uint64(*e.RequestedCompressedSize), uint64(e.UncompressedSize)),
					e.requestedMethod(),
				})
			}
			writeTable(&sb, "  ",
				[]string{"PATH", "SIZE", "COMPRESSED", "RATIO", "REQUESTED"},
				[]bool{false, true, true, true, false},
				rows)
		}
	}

	// NOTE: Duplicate content is detectable only with content hashes from the debug index.
	if info.Seed != nil {
		sb.WriteString("\n")
		if len(info.Duplicates) == 0 {
			sb.WriteString("Identical content stored more than once: none\n")
		} else {
			sb.WriteString(fmt.Sprintf("Identical content stored more than once: %d groups\n", len(info.Duplicates)))
			for _, d := range info.Duplicates {
				sb.WriteString(fmt.Sprintf("  content hash 0x%016x, %s, %d files:\n", d.ContentHash, formatSize(uint64(d.Size)), len(d.Paths)))
				for _, path := range d.Paths {
					sb.WriteString(fmt.Sprintf("    %s\n", path))
				}
			}
		}
	}

	// NOTE: Diagnostics need the requested dictionaries from the debug index.
	if info.Seed != nil {
		sb.WriteString("\n")
		if len(info.Diagnostics) == 0 {
			sb.WriteString("Diagnostics: none\n")
		} else {
			sb.WriteString(fmt.Sprintf("Diagnostics: %d\n", len(info.Diagnostics)))
			for _, d := range info.Diagnostics {
				sb.WriteString(fmt.Sprintf("  %s: %s\n", d.Kind, d.Message))
			}
		}
	}

	return sb.String()
}

// formatRatio returns compressed as a percentage of uncompressed.
func formatRatio(compressed, uncompressed uint64) string {
	if uncompressed == 0 {
		return "-"
	}
	return fmt.Sprintf("%.1f%%", float64(compressed)*100/float64(uncompressed))
}

func formatSize(bytes uint64) string {
	switch {
	case bytes >= 1024*1024*1024:
		return fmt.Sprintf("%.2f GiB", float64(bytes)/(1024*1024*1024))
	case bytes >= 1024*1024:
		return fmt.Sprintf("%.2f MiB", float64(bytes)/(1024*1024))
	case bytes >= 1024:
		return fmt.Sprintf("%.2f KiB", float64(bytes)/1024)
	default:
		return fmt.Sprintf("%d B", bytes)
	}
}
