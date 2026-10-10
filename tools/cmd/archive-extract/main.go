// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"github.com/OneOfOne/xxhash"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

func main() {
	env := Env{}
	flag.StringVar(&env.IndexFile, "idx", "", "input index file (*.idx)")
	flag.StringVar(&env.PakFile, "pak", "", "input binary archive file (*.pak)")
	flag.StringVar(&env.DebugFile, "debug", "", "input debug index file (*.idx-debug); restores paths and verifies content hashes")
	flag.StringVar(&env.OutDir, "o", "", "output directory")
	flag.StringVar(&env.ZstdCommand, "zstd", "", "zstd command-line tool, required to decode zstd entries")
	flag.Parse()

	env.Paths = flag.Args()

	if env.IndexFile == "" {
		log.Fatal("-idx is required")
	}
	if env.PakFile == "" {
		log.Fatal("-pak is required")
	}
	if env.OutDir == "" {
		log.Fatal("-o is required")
	}

	if err := run(&env); err != nil {
		log.Fatal(err)
	}
}

type Env struct {
	IndexFile   string
	PakFile     string
	DebugFile   string
	OutDir      string
	ZstdCommand string
	Paths       []string // positional args: paths as listed in the archive recipes
}

// extractTarget is an archive entry to restore and the file to write it to.
type extractTarget struct {
	name    string // path as listed in the recipe, or the key in hex
	outPath string
	entry   *schemas.ArchiveEntryT
	debug   *schemas.ArchiveKeyDebugT // nil without a debug index
}

func run(env *Env) error {
	// NOTE: Read the index file (.idx).
	idxBuf, err := os.ReadFile(env.IndexFile)
	if err != nil {
		return fmt.Errorf("failed to read index file: %w", err)
	}
	archive := schemas.GetRootAsArchive(idxBuf, 0).UnPack()

	entries := map[uint64]*schemas.ArchiveEntryT{}
	for _, key := range archive.Keys {
		if int(key.Index) >= len(archive.Entries) {
			return fmt.Errorf("key 0x%016x references out-of-range entry index %d", key.Key, key.Index)
		}
		entries[key.Key] = archive.Entries[key.Index]
	}

	// NOTE: Read the debug index file (.idx-debug) if provided.
	debugKeys := map[uint64]*schemas.ArchiveKeyDebugT{}
	if env.DebugFile != "" {
		debugBuf, err := os.ReadFile(env.DebugFile)
		if err != nil {
			return fmt.Errorf("failed to read debug file: %w", err)
		}
		debugArchive := schemas.GetRootAsArchiveDebug(debugBuf, 0).UnPack()
		if debugArchive.Seed != stringhash.Seed64 {
			return fmt.Errorf("debug index uses seed %d, but this tool hashes paths with seed %d", debugArchive.Seed, stringhash.Seed64)
		}
		for _, dk := range debugArchive.Keys {
			debugKeys[dk.Key] = dk
		}
	}

	// NOTE: Collect the entries to extract. Without positional paths, every entry is extracted.
	targets := []*extractTarget{}
	addTarget := func(name string, key uint64) error {
		entry, ok := entries[key]
		if !ok {
			return fmt.Errorf("not found in archive: %s (key 0x%016x)", name, key)
		}
		outPath, err := outputPath(env.OutDir, name)
		if err != nil {
			return err
		}
		target := &extractTarget{name: name, outPath: outPath, entry: entry}
		if env.DebugFile != "" {
			dk, ok := debugKeys[key]
			if !ok || dk.Path != name {
				return fmt.Errorf("not found in debug index: %s (key 0x%016x)", name, key)
			}
			target.debug = dk
		}
		targets = append(targets, target)
		return nil
	}

	if len(env.Paths) > 0 {
		for _, path := range env.Paths {
			if err := addTarget(path, stringhash.StringToHash64(path)); err != nil {
				return err
			}
		}
	} else {
		for _, key := range archive.Keys {
			name := fmt.Sprintf("%016x", key.Key)
			if env.DebugFile != "" {
				dk, ok := debugKeys[key.Key]
				if !ok {
					return fmt.Errorf("key 0x%016x has no path in the debug index", key.Key)
				}
				name = dk.Path
			}
			if err := addTarget(name, key.Key); err != nil {
				return err
			}
		}
	}

	// NOTE: Restore each entry from the .pak file.
	pak, err := os.Open(env.PakFile)
	if err != nil {
		return fmt.Errorf("failed to open pak file: %w", err)
	}
	defer pak.Close()

	stat, err := pak.Stat()
	if err != nil {
		return fmt.Errorf("pak.Stat() failed: %w", err)
	}

	needsDictionaries := false
	for _, target := range targets {
		switch target.entry.Compression {
		case schemas.ArchiveCompressionZstd, schemas.ArchiveCompressionZstdDictionary:
			if env.ZstdCommand == "" {
				return fmt.Errorf("-zstd is required to decode %s, which is stored as a zstd frame", target.name)
			}
		}
		if target.entry.Compression == schemas.ArchiveCompressionZstdDictionary {
			needsDictionaries = true
		}
	}

	decoder := &zstdDecoder{command: env.ZstdCommand}

	// NOTE: The zstd command reads dictionaries from files.
	if needsDictionaries {
		dir, err := os.MkdirTemp("", "archive-extract-")
		if err != nil {
			return fmt.Errorf("os.MkdirTemp() failed: %w", err)
		}
		defer os.RemoveAll(dir)

		for i, d := range archive.Dictionaries {
			path := filepath.Join(dir, fmt.Sprintf("%d.dict", i))
			if err := os.WriteFile(path, d.Data, 0o644); err != nil {
				return fmt.Errorf("os.WriteFile() failed: %w", err)
			}
			decoder.dictionaries = append(decoder.dictionaries, path)
		}
	}

	totalBytes := uint64(0)
	for _, target := range targets {
		data, err := readEntry(pak, uint64(stat.Size()), decoder, target.entry)
		if err != nil {
			return fmt.Errorf("%s: %w", target.name, err)
		}

		if target.debug != nil {
			if hash := xxhash.Checksum64(data); hash != target.debug.ContentHash {
				return fmt.Errorf("%s: content hash 0x%016x does not match 0x%016x in the debug index", target.name, hash, target.debug.ContentHash)
			}
		}

		if err := os.MkdirAll(filepath.Dir(target.outPath), os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
		if err := os.WriteFile(target.outPath, data, 0o644); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
		totalBytes += uint64(len(data))
	}

	fmt.Printf("Extracted %d files (%d bytes) to %s\n", len(targets), totalBytes, env.OutDir)

	return nil
}

// outputPath joins name to dir, rejecting names that would escape dir.
func outputPath(dir, name string) (string, error) {
	p := filepath.FromSlash(name)
	if !filepath.IsLocal(p) {
		return "", fmt.Errorf("refusing to write outside the output directory: %s", name)
	}
	return filepath.Join(dir, p), nil
}

// zstdDecoder decodes zstd frames with the zstd command-line tool.
type zstdDecoder struct {
	command string

	// dictionaries lists the files that hold the dictionaries of the archive index.
	dictionaries []string
}

// decode returns the content of frame, which must hold uncompressedSize bytes.
// It decodes with the dictionary at dictionaryIndex unless dictionaryIndex is negative.
func (d *zstdDecoder) decode(frame []byte, uncompressedSize uint32, dictionaryIndex int) ([]byte, error) {
	args := []string{"-d", "-q", "-c"}
	if dictionaryIndex >= 0 {
		if dictionaryIndex >= len(d.dictionaries) {
			return nil, fmt.Errorf("out-of-range dictionary index %d", dictionaryIndex)
		}
		args = append(args, "-D", d.dictionaries[dictionaryIndex])
	}

	var stdout, stderr bytes.Buffer
	cmd := exec.Command(d.command, args...)
	cmd.Stdin = bytes.NewReader(frame)
	cmd.Stdout = &stdout
	cmd.Stderr = &stderr
	if err := cmd.Run(); err != nil {
		return nil, fmt.Errorf("zstd decoding failed: %w: %s", err, strings.TrimSpace(stderr.String()))
	}

	if stdout.Len() != int(uncompressedSize) {
		return nil, fmt.Errorf("zstd entry decodes to %d bytes, but the index records %d bytes", stdout.Len(), uncompressedSize)
	}
	return stdout.Bytes(), nil
}

// readEntry reads the stored bytes of entry from the .pak file and returns the
// original file content.
func readEntry(pak *os.File, pakSize uint64, decoder *zstdDecoder, entry *schemas.ArchiveEntryT) ([]byte, error) {
	end := uint64(entry.StartOffset) + uint64(entry.CompressedSize)
	if end > pakSize {
		return nil, fmt.Errorf("entry ends at byte %d, beyond the pak size %d", end, pakSize)
	}

	stored := make([]byte, entry.CompressedSize)
	if _, err := pak.ReadAt(stored, int64(entry.StartOffset)); err != nil {
		return nil, fmt.Errorf("pak.ReadAt() failed: %w", err)
	}

	switch entry.Compression {
	case schemas.ArchiveCompressionNone:
		if entry.CompressedSize != entry.UncompressedSize {
			return nil, fmt.Errorf("uncompressed entry stores %d bytes for a %d-byte file", entry.CompressedSize, entry.UncompressedSize)
		}
		return stored, nil
	case schemas.ArchiveCompressionZstd:
		return decoder.decode(stored, entry.UncompressedSize, -1)
	case schemas.ArchiveCompressionZstdDictionary:
		return decoder.decode(stored, entry.UncompressedSize, int(entry.DictionaryIndex))
	}
	return nil, fmt.Errorf("unsupported compression %v", entry.Compression)
}
