// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"
	"path/filepath"
	"sort"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

func buildKey(uuid, platform string) uint64 {
	return stringhash.StringToHash64(uuid + ":" + platform)
}

func main() {
	inFile := ""
	outFile := ""
	outDebugFile := ""
	flag.StringVar(&inFile, "i", "", "input SDL_GameControllerDB file (gamecontrollerdb.txt)")
	flag.StringVar(&outFile, "o", "", "output game controller DB file (*.gcdb)")
	flag.StringVar(&outDebugFile, "outdebug", "", "output debug file (*.gcdb-debug, optional)")
	flag.Parse()

	if inFile == "" {
		log.Fatal("-i is required")
	}
	if outFile == "" {
		log.Fatal("-o is required")
	}

	if err := run(inFile, outFile, outDebugFile); err != nil {
		log.Fatal(err)
	}
}

func run(inFile, outFile, outDebugFile string) error {
	entries, err := parseSDLGameControllerDB(inFile)
	if err != nil {
		return fmt.Errorf("parseSDLGameControllerDB() failed: %w", err)
	}

	// NOTE: Detect hash collisions
	hashToEntry := map[uint64]string{}

	db := &schemas.GameControllerDBT{}
	debugDB := &schemas.GameControllerDBDebugT{
		Seed: stringhash.Seed64,
	}

	for _, entry := range entries {
		key := buildKey(entry.uuid, entry.platform)

		label := fmt.Sprintf("%s:%s (%s)", entry.uuid, entry.platform, entry.name)
		if existing, ok := hashToEntry[key]; ok {
			return fmt.Errorf("hash collision detected: %q and %q produce the same xxHash-64 (0x%016x)", existing, label, key)
		}
		hashToEntry[key] = label

		// NOTE: Build buttons slice
		buttons := make([]int8, numButtons)
		copy(buttons, entry.buttons[:])

		// NOTE: Build axes
		axes := make([]*schemas.GameControllerAxisMappingT, numAxes)
		for i := range entry.axes {
			axes[i] = &schemas.GameControllerAxisMappingT{
				ThumbStick:      entry.axes[i].thumbStick,
				PositiveTrigger: entry.axes[i].positiveTrigger,
				NegativeTrigger: entry.axes[i].negativeTrigger,
			}
		}

		// NOTE: Build hats slice
		hats := make([]int8, numHats)
		copy(hats, entry.hats[:])

		mapping := &schemas.GameControllerMappingT{
			Key:     key,
			Name:    entry.name,
			Buttons: buttons,
			Axes:    axes,
			Hats:    hats,
		}
		db.Mappings = append(db.Mappings, mapping)

		debugMapping := &schemas.GameControllerMappingDebugT{
			Key:      key,
			Uuid:     entry.uuid,
			Name:     entry.name,
			Platform: entry.platform,
		}
		debugDB.Mappings = append(debugDB.Mappings, debugMapping)
	}

	// NOTE: Sort by key for LookupByKey() binary search
	sort.SliceStable(db.Mappings, func(i, j int) bool {
		return db.Mappings[i].Key < db.Mappings[j].Key
	})
	sort.SliceStable(debugDB.Mappings, func(i, j int) bool {
		return debugDB.Mappings[i].Key < debugDB.Mappings[j].Key
	})

	// NOTE: Create output directory
	if dir := filepath.Dir(outFile); dir != "" {
		if err := os.MkdirAll(dir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	// NOTE: Write main binary
	{
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(db.Pack(builder))

		if err := os.WriteFile(outFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	// NOTE: Write debug binary (optional)
	if outDebugFile != "" {
		if dir := filepath.Dir(outDebugFile); dir != "" {
			if err := os.MkdirAll(dir, os.ModePerm); err != nil {
				return fmt.Errorf("os.MkdirAll() failed: %w", err)
			}
		}

		builder := flatbuffers.NewBuilder(0)
		builder.Finish(debugDB.Pack(builder))

		if err := os.WriteFile(outDebugFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	return nil
}
