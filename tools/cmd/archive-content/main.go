// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"crypto/sha256"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/OneOfOne/xxhash"
	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	archives "github.com/mogemimi/pomdog/tools/pkg/archives"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
	zstdframe "github.com/mogemimi/pomdog/tools/pkg/zstdframe"
)

func main() {
	env := Env{}
	flag.StringVar(&env.OutIndexFile, "o", "", "output file (*.idx)")
	flag.StringVar(&env.OutBinaryFile, "outbin", "", "output file (*.pak)")
	flag.StringVar(&env.OutDebugFile, "outdebug", "", "output debug file (*.idx-debug)")
	flag.StringVar(&env.ContentDir, "contentdir", "", "content directory")
	flag.Var(&env.TargetPlatforms, "platform", "target platform (repeatable)")
	flag.StringVar(&env.SettingsFile, "settings", "", "archive settings file (*.toml)")
	flag.StringVar(&env.ZstdDir, "zstddir", "", "directory of the zstd frames and dictionaries for the files that request zstd")
	flag.Parse()

	if env.SettingsFile == "" {
		log.Fatal("--settings is required")
	}
	if len(env.TargetPlatforms) == 0 {
		log.Fatal("--platform is required to select a strategy from the archive settings")
	}

	settings := archives.ArchiveSettings{}
	if err := settings.ReadFile(env.SettingsFile); err != nil {
		log.Fatal(err)
	}

	recipe := archives.ArchiveRecipe{}
	if err := recipe.ReadFiles(flag.Args()); err != nil {
		log.Fatal(err)
	}

	if err := run(&env, &settings, &recipe); err != nil {
		log.Fatal(err)
	}
}

type Env struct {
	OutIndexFile    string
	OutBinaryFile   string
	OutDebugFile    string
	ContentDir      string
	SettingsFile    string
	ZstdDir         string
	TargetPlatforms stringSlice
}

// stringSlice implements flag.Value for repeated string flags.
type stringSlice []string

func (s *stringSlice) String() string { return strings.Join(*s, ", ") }
func (s *stringSlice) Set(v string) error {
	*s = append(*s, v)
	return nil
}

// entryKey identifies an archive entry by everything that it stores, so that
// keys of files stored in the same way share one entry.
type entryKey struct {
	startOffset      uint32
	uncompressedSize uint32
	compressedSize   uint32
	compression      schemas.ArchiveCompression
	dictionaryIndex  uint8
}

func run(env *Env, settings *archives.ArchiveSettings, recipe *archives.ArchiveRecipe) error {
	plan, err := archives.PlanArchive(settings, recipe, env.TargetPlatforms)
	if err != nil {
		return fmt.Errorf("%s: %w", env.SettingsFile, err)
	}

	for _, file := range plan.Files {
		if file.Compression == archives.CompressionZstd && env.ZstdDir == "" {
			return fmt.Errorf("--zstddir is required to pack %s, which requests zstd", file.Path)
		}
	}

	// NOTE: Read the dictionaries that the zstd frames of the files use.
	dictionaryData := map[*archives.PlannedDictionary][]byte{}
	for _, d := range plan.Dictionaries {
		data, err := readZstdDictionary(env.ZstdDir, d)
		if err != nil {
			return err
		}
		dictionaryData[d] = data
	}

	archive := &schemas.ArchiveT{}
	debugArchive := &schemas.ArchiveDebugT{
		Seed: stringhash.Seed64,
	}

	// NOTE: Write the .pak and .idx files to temporary files and rename them over
	// the outputs at the end. A running game that maps the old archive keeps
	// reading it, instead of seeing the file truncated under it.
	binFile, err := createTempFile(env.OutBinaryFile)
	if err != nil {
		return err
	}
	defer os.Remove(binFile.Name())
	defer binFile.Close()

	// NOTE: Detect hash collisions: map from hash to the first path that produced it.
	hashToPath := map[uint64]string{}

	// NOTE: Files whose stored bytes are identical share them in the .pak file,
	// even if they restore different content. Files stored in the same way also
	// share one entry.
	storedOffsets := map[[sha256.Size]byte]uint32{}
	entryIndices := map[entryKey]uint32{}

	// NOTE: The index holds only the dictionaries that the stored entries use.
	dictionaryIndices := map[*archives.PlannedDictionary]uint8{}

	totalBytes := uint64(0)

	for _, file := range plan.Files {
		data, err := os.ReadFile(filepath.Join(env.ContentDir, file.Path))
		if err != nil {
			return fmt.Errorf("os.ReadFile() failed: %w", err)
		}
		if uint64(len(data)) > math.MaxUint32 {
			return fmt.Errorf("file is larger than 4 GiB: %s", file.Path)
		}

		// NOTE: Store the file as-is unless the requested method saves enough
		// to be worth decoding.
		stored := data
		compression := schemas.ArchiveCompressionNone
		var dictionary *archives.PlannedDictionary

		requestedCompression := schemas.ArchiveCompressionNone
		requestedCompressedSize := len(data)
		requestedDictionary := ""

		switch file.Compression {
		case archives.CompressionNone:
		case archives.CompressionZstd:
			frame, err := readZstdFrame(env.ZstdDir, file, len(data))
			if err != nil {
				return err
			}

			requestedCompression = schemas.ArchiveCompressionZstd
			if file.Dictionary != nil {
				requestedCompression = schemas.ArchiveCompressionZstdDictionary
				requestedDictionary = file.Dictionary.Settings.Name
			}
			requestedCompressedSize = len(frame)

			if plan.Strategy.Zstd.Accepts(len(data), len(frame)) {
				stored = frame
				compression = requestedCompression
				dictionary = file.Dictionary
			}
		default:
			return fmt.Errorf("unsupported compression %q: %s", file.Compression, file.Path)
		}

		storedHash := sha256.Sum256(stored)
		startOffset, ok := storedOffsets[storedHash]
		if !ok {
			if totalBytes+uint64(len(stored)) > math.MaxUint32 {
				return fmt.Errorf("archive data exceeds 4 GiB at %s", file.Path)
			}
			if _, err := binFile.Write(stored); err != nil {
				return fmt.Errorf("binFile.Write() failed: %w", err)
			}
			startOffset = uint32(totalBytes)
			storedOffsets[storedHash] = startOffset
			totalBytes += uint64(len(stored))
		}

		dictionaryIndex := uint8(0)
		if dictionary != nil {
			index, ok := dictionaryIndices[dictionary]
			if !ok {
				// NOTE: The archive settings allow no more dictionaries than an 8-bit index refers to.
				if len(archive.Dictionaries) > math.MaxUint8 {
					return fmt.Errorf("the archive needs more than %d dictionaries at %s", math.MaxUint8+1, file.Path)
				}
				index = uint8(len(archive.Dictionaries))
				dictionaryIndices[dictionary] = index
				archive.Dictionaries = append(archive.Dictionaries, &schemas.ArchiveDictionaryT{
					Data: dictionaryData[dictionary],
				})
				debugArchive.Dictionaries = append(debugArchive.Dictionaries, &schemas.ArchiveDictionaryDebugT{
					Name: dictionary.Settings.Name,
				})
			}
			dictionaryIndex = index
		}

		entry := entryKey{
			startOffset:      startOffset,
			uncompressedSize: uint32(len(data)),
			compressedSize:   uint32(len(stored)),
			compression:      compression,
			dictionaryIndex:  dictionaryIndex,
		}
		entryIndex, ok := entryIndices[entry]
		if !ok {
			entryIndex = uint32(len(archive.Entries))
			entryIndices[entry] = entryIndex
			archive.Entries = append(archive.Entries, &schemas.ArchiveEntryT{
				StartOffset:      entry.startOffset,
				UncompressedSize: entry.uncompressedSize,
				CompressedSize:   entry.compressedSize,
				Compression:      entry.compression,
				DictionaryIndex:  entry.dictionaryIndex,
			})
		}

		hash := stringhash.StringToHash64(file.Path)

		if existingPath, ok := hashToPath[hash]; ok {
			return fmt.Errorf("hash collision detected: %q and %q produce the same xxHash-64 (0x%016x)", existingPath, file.Path, hash)
		}
		hashToPath[hash] = file.Path

		archive.Keys = append(archive.Keys, &schemas.ArchiveKeyT{
			Key:   hash,
			Index: entryIndex,
		})

		debugArchive.Keys = append(debugArchive.Keys, &schemas.ArchiveKeyDebugT{
			Path:                    file.Path,
			Key:                     hash,
			ContentHash:             xxhash.Checksum64(data),
			RequestedCompression:    requestedCompression,
			RequestedCompressedSize: uint32(requestedCompressedSize),
			RequestedDictionary:     requestedDictionary,
		})
	}

	sort.SliceStable(archive.Keys, func(i, j int) bool {
		return archive.Keys[i].Key < archive.Keys[j].Key
	})
	sort.SliceStable(debugArchive.Keys, func(i, j int) bool {
		return debugArchive.Keys[i].Key < debugArchive.Keys[j].Key
	})

	if err := binFile.Close(); err != nil {
		return fmt.Errorf("binFile.Close() failed: %w", err)
	}
	if err := replaceFile(binFile.Name(), env.OutBinaryFile); err != nil {
		return err
	}

	{
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(archive.Pack(builder))

		if err := writeFileByRename(env.OutIndexFile, builder.FinishedBytes()); err != nil {
			return err
		}
	}

	if env.OutDebugFile != "" {
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(debugArchive.Pack(builder))

		if err := os.WriteFile(env.OutDebugFile, builder.FinishedBytes(), 0o644); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	return nil
}

// createTempFile creates a temporary file next to path, for replaceFile to
// rename over path once it is written.
func createTempFile(path string) (*os.File, error) {
	file, err := os.CreateTemp(filepath.Dir(path), filepath.Base(path)+".*.tmp")
	if err != nil {
		return nil, fmt.Errorf("os.CreateTemp() failed: %w", err)
	}
	// NOTE: os.CreateTemp() creates the file readable only by its owner.
	if err := file.Chmod(0o644); err != nil {
		file.Close()
		os.Remove(file.Name())
		return nil, fmt.Errorf("file.Chmod() failed: %w", err)
	}
	return file, nil
}

// replaceFile renames the file at tempPath over path. A process that has the
// old file at path open keeps reading the old content.
func replaceFile(tempPath, path string) error {
	if err := renameFile(tempPath, path); err != nil {
		return fmt.Errorf("renameFile() failed: %w", err)
	}
	return nil
}

// writeFileByRename writes data to a temporary file next to path and renames
// it over path.
func writeFileByRename(path string, data []byte) error {
	file, err := createTempFile(path)
	if err != nil {
		return err
	}
	defer os.Remove(file.Name())

	if _, err := file.Write(data); err != nil {
		file.Close()
		return fmt.Errorf("file.Write() failed: %w", err)
	}
	if err := file.Close(); err != nil {
		return fmt.Errorf("file.Close() failed: %w", err)
	}
	return replaceFile(file.Name(), path)
}

// readZstdDictionary reads dictionary d from zstdDir and checks its dictionary ID.
func readZstdDictionary(zstdDir string, d *archives.PlannedDictionary) ([]byte, error) {
	path := archives.ZstdDictionaryPath(zstdDir, d.ID)
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}
	id, err := zstdframe.DictionaryID(data)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", path, err)
	}
	if id != d.ID {
		return nil, fmt.Errorf("%s: dictionary ID is %d, want %d", path, id, d.ID)
	}
	return data, nil
}

// readZstdFrame reads the zstd frame of file from zstdDir and checks that the
// frame records the content size and the dictionary ID of the file.
func readZstdFrame(zstdDir string, file *archives.PlannedFile, contentSize int) ([]byte, error) {
	path := archives.ZstdFramePath(zstdDir, file.Path)
	frame, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}
	if uint64(len(frame)) > math.MaxUint32 {
		return nil, fmt.Errorf("zstd frame is larger than 4 GiB: %s", path)
	}

	header, err := zstdframe.ParseHeader(frame)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", path, err)
	}
	if !header.HasContentSize {
		return nil, fmt.Errorf("%s: the frame does not record the content size", path)
	}
	if header.ContentSize != uint64(contentSize) {
		return nil, fmt.Errorf("%s: the frame holds %d bytes, but %s has %d bytes", path, header.ContentSize, file.Path, contentSize)
	}

	dictionaryID := uint32(0)
	if file.Dictionary != nil {
		dictionaryID = file.Dictionary.ID
	}
	if header.DictionaryID != dictionaryID {
		return nil, fmt.Errorf("%s: the frame records dictionary ID %d, want %d", path, header.DictionaryID, dictionaryID)
	}
	return frame, nil
}
