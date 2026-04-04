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
)

func main() {
	env := Env{}
	flag.StringVar(&env.IndexFile, "idx", "", "input index file (*.idx)")
	flag.StringVar(&env.PakFile, "pak", "", "input binary archive file (*.pak)")
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

// entryInfo holds the merged information for a single archive entry.
type entryInfo struct {
	Path             string `json:"path,omitempty"`
	Key              uint64 `json:"key"`
	Index            uint32 `json:"index"`
	StartOffset      uint32 `json:"start_offset"`
	UncompressedSize uint32 `json:"uncompressed_size"`
	CompressedSize   uint32 `json:"compressed_size"`
	Compressed       bool   `json:"compressed"`
}

// archiveInfo is the top-level JSON output structure.
type archiveInfo struct {
	PakFile    string       `json:"pak_file,omitempty"`
	TotalFiles int          `json:"total_files"`
	TotalBytes uint64       `json:"total_bytes"`
	Seed       *uint64      `json:"seed,omitempty"`
	Entries    []*entryInfo `json:"entries"`
}

func run(env *Env) error {
	// NOTE: Read the index file (.idx).
	idxBuf, err := os.ReadFile(env.IndexFile)
	if err != nil {
		return fmt.Errorf("failed to read index file: %w", err)
	}
	archive := schemas.GetRootAsArchive(idxBuf, 0)
	archiveT := archive.UnPack()

	// NOTE: Build a map from key hash to debug path (if debug file is provided).
	debugPaths := map[uint64]string{}
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
			debugPaths[dk.Key] = dk.Path
		}
	}

	// NOTE: Merge keys and entries into a unified list sorted by entry index.
	entries := make([]*entryInfo, 0, len(archiveT.Keys))
	for _, key := range archiveT.Keys {
		idx := key.Index
		if int(idx) >= len(archiveT.Entries) {
			return fmt.Errorf("key 0x%016x references out-of-range entry index %d", key.Key, idx)
		}
		e := archiveT.Entries[idx]
		info := &entryInfo{
			Path:             debugPaths[key.Key],
			Key:              key.Key,
			Index:            idx,
			StartOffset:      e.StartOffset,
			UncompressedSize: e.UncompressedSize,
			CompressedSize:   e.CompressedSize,
			Compressed:       e.Compressed,
		}
		entries = append(entries, info)
	}

	// NOTE: Sort by path if available, otherwise by key.
	sort.SliceStable(entries, func(i, j int) bool {
		if entries[i].Path != "" && entries[j].Path != "" {
			return entries[i].Path < entries[j].Path
		}
		return entries[i].Key < entries[j].Key
	})

	// NOTE: Compute total bytes.
	var totalBytes uint64
	for _, e := range entries {
		totalBytes += uint64(e.CompressedSize)
	}

	info := &archiveInfo{
		PakFile:    env.PakFile,
		TotalFiles: len(entries),
		TotalBytes: totalBytes,
		Seed:       seed,
		Entries:    entries,
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

func formatText(info *archiveInfo) string {
	var sb strings.Builder

	sb.WriteString(fmt.Sprintf("Archive: %d files, %s\n", info.TotalFiles, formatSize(info.TotalBytes)))
	if info.PakFile != "" {
		sb.WriteString(fmt.Sprintf("Pak: %s\n", info.PakFile))
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
		sb.WriteString(fmt.Sprintf("%-70s %18s %10s %10s %s\n", "PATH", "KEY", "OFFSET", "SIZE", "COMPRESSED"))
		sb.WriteString(fmt.Sprintf("%s %s %s %s %s\n",
			strings.Repeat("-", 70),
			strings.Repeat("-", 18),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
		))
		for _, e := range info.Entries {
			compressed := ""
			if e.Compressed {
				compressed = fmt.Sprintf("yes (%s)", formatSize(uint64(e.CompressedSize)))
			}
			sb.WriteString(fmt.Sprintf("%-70s 0x%016x %10d %10d %s\n",
				e.Path, e.Key, e.StartOffset, e.UncompressedSize, compressed))
		}
	} else {
		sb.WriteString(fmt.Sprintf("%18s %10s %10s %s\n", "KEY", "OFFSET", "SIZE", "COMPRESSED"))
		sb.WriteString(fmt.Sprintf("%s %s %s %s\n",
			strings.Repeat("-", 18),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
			strings.Repeat("-", 10),
		))
		for _, e := range info.Entries {
			compressed := ""
			if e.Compressed {
				compressed = fmt.Sprintf("yes (%s)", formatSize(uint64(e.CompressedSize)))
			}
			sb.WriteString(fmt.Sprintf("0x%016x %10d %10d %s\n",
				e.Key, e.StartOffset, e.UncompressedSize, compressed))
		}
	}

	return sb.String()
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
