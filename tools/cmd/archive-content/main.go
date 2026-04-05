// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"io"
	"io/fs"
	"log"
	"os"
	"path/filepath"
	"sort"
	"strings"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	archives "github.com/mogemimi/pomdog/tools/pkg/archives"
	depfile "github.com/mogemimi/pomdog/tools/pkg/depfile"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

func main() {
	env := Env{}
	flag.StringVar(&env.OutIndexFile, "o", "", "output file (*.idx)")
	flag.StringVar(&env.OutBinaryFile, "outbin", "", "output file (*.pak)")
	flag.StringVar(&env.OutDebugFile, "outdebug", "", "output debug file (*.idx-debug)")
	flag.StringVar(&env.ContentDir, "contentdir", "", "content directory")
	flag.Var(&env.TargetPlatforms, "platform", "target platform (repeatable)")
	flag.StringVar(&env.OutDepFile, "depfile", "", "output depfile for ninja (optional)")
	flag.Parse()

	recipe := archives.ArchiveRecipe{}
	if err := recipe.ReadFiles(flag.Args()); err != nil {
		log.Fatal(err)
	}

	if err := run(&env, &recipe); err != nil {
		log.Fatal(err)
	}
}

type Env struct {
	OutIndexFile    string
	OutBinaryFile   string
	OutDebugFile    string
	OutDepFile      string
	ContentDir      string
	TargetPlatforms stringSlice
}

// stringSlice implements flag.Value for repeated string flags.
type stringSlice []string

func (s *stringSlice) String() string { return strings.Join(*s, ", ") }
func (s *stringSlice) Set(v string) error {
	*s = append(*s, v)
	return nil
}

func run(env *Env, recipe *archives.ArchiveRecipe) error {
	archive := &schemas.ArchiveT{}
	debugArchive := &schemas.ArchiveDebugT{
		Seed: stringhash.Seed64,
	}

	binFile, err := os.Create(env.OutBinaryFile)
	if err != nil {
		return fmt.Errorf("os.Create() failed: %w", err)
	}
	defer binFile.Close()

	totalBytes := uint32(0)
	index := uint32(0)

	addedFiles := map[string]struct{}{}

	filePaths := []string{}
	for _, group := range recipe.Group {
		if len(group.TargetPlatforms) > 0 && len(env.TargetPlatforms) > 0 {
			found := func() bool {
				for _, p := range group.TargetPlatforms {
					for _, tp := range env.TargetPlatforms {
						if p == tp {
							return true
						}
					}
				}
				return false
			}()
			if !found {
				continue
			}
		}
		for _, path := range group.Paths {
			if _, ok := addedFiles[path]; ok {
				return fmt.Errorf("already exists: %v", path)
			}
			filePaths = append(filePaths, path)
			addedFiles[path] = struct{}{}
		}
	}
	sort.SliceStable(filePaths, func(i, j int) bool {
		return filePaths[i] < filePaths[j]
	})

	// NOTE: Detect hash collisions: map from hash to the first path that produced it.
	hashToPath := map[uint64]string{}

	for _, path := range filePaths {
		src, err := os.Open(filepath.Join(env.ContentDir, path))
		if err != nil {
			return fmt.Errorf("os.Open() failed: %w", err)
		}
		defer src.Close()

		nBytes, err := io.Copy(binFile, src)
		if err != nil {
			return fmt.Errorf("io.Copy() failed: %w", err)
		}

		hash := stringhash.StringToHash64(path)

		if existingPath, ok := hashToPath[hash]; ok {
			return fmt.Errorf("hash collision detected: %q and %q produce the same xxHash-64 (0x%016x)", existingPath, path, hash)
		}
		hashToPath[hash] = path

		arEntry := &schemas.ArchiveEntryT{
			StartOffset:      totalBytes,
			CompressedSize:   uint32(nBytes),
			UncompressedSize: uint32(nBytes),
			Compressed:       false,
		}

		arKey := &schemas.ArchiveKeyT{
			Key:   hash,
			Index: index,
		}

		archive.Keys = append(archive.Keys, arKey)
		archive.Entries = append(archive.Entries, arEntry)

		debugArchive.Keys = append(debugArchive.Keys, &schemas.ArchiveKeyDebugT{
			Path: path,
			Key:  hash,
		})

		totalBytes += uint32(nBytes)
		index += 1
	}

	sort.SliceStable(archive.Keys, func(i, j int) bool {
		return archive.Keys[i].Key < archive.Keys[j].Key
	})
	sort.SliceStable(debugArchive.Keys, func(i, j int) bool {
		return debugArchive.Keys[i].Path < debugArchive.Keys[j].Path
	})

	{
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(archive.Pack(builder))

		if err := os.WriteFile(env.OutIndexFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	if env.OutDebugFile != "" {
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(debugArchive.Pack(builder))

		if err := os.WriteFile(env.OutDebugFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	if env.OutDepFile != "" {
		depFiles := make([]string, 0, len(filePaths))
		for _, path := range filePaths {
			depFiles = append(depFiles, filepath.Join(env.ContentDir, path))
		}

		dep := &depfile.DepFile{
			OutFile:  env.OutIndexFile,
			DepFiles: depFiles,
		}
		if err := dep.WriteFile(env.OutDepFile, fs.ModePerm); err != nil {
			return fmt.Errorf("depfile.WriteFile() failed: %w", err)
		}
	}

	return nil
}
