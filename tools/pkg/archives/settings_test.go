// Copyright mogemimi. Distributed under the MIT license.

package archives

import (
	"fmt"
	"strings"
	"testing"
)

const testSettings = `[[strategy]]
target_platforms = ["windows", "macos", "linux"]
compression = "none"

[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 1024
max_ratio_percent = 90
`

// testZstdStrategy is a web strategy to which tests append dictionaries.
const testZstdStrategy = `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 32
max_ratio_percent = 95
`

func TestSettingsStrategyFor(t *testing.T) {
	settings := ArchiveSettings{}
	if err := settings.ReadFile(writeRecipeFile(t, testSettings)); err != nil {
		t.Fatalf("ReadFile() failed: %v", err)
	}

	for platform, want := range map[string]string{
		PlatformWindows: CompressionNone,
		PlatformMacOS:   CompressionNone,
		PlatformLinux:   CompressionNone,
		PlatformWeb:     CompressionZstd,
	} {
		strategy, err := settings.StrategyFor(platform)
		if err != nil {
			t.Fatalf("StrategyFor(%q) failed: %v", platform, err)
		}
		if strategy.Compression != want {
			t.Errorf("StrategyFor(%q).Compression = %q, want %q", platform, strategy.Compression, want)
		}
	}

	strategy, err := settings.StrategyFor(PlatformWeb)
	if err != nil {
		t.Fatalf("StrategyFor() failed: %v", err)
	}
	if *strategy.Zstd.Level != 19 || *strategy.Zstd.MinSavedBytes != 1024 || *strategy.Zstd.MaxRatioPercent != 90 {
		t.Errorf("unexpected zstd settings: %d, %d, %v", *strategy.Zstd.Level, *strategy.Zstd.MinSavedBytes, *strategy.Zstd.MaxRatioPercent)
	}

	if _, err := settings.StrategyFor("emscripten"); err == nil {
		t.Error("StrategyFor(\"emscripten\") succeeded, want an error for an unknown platform")
	}
}

func TestSettingsMissingPlatform(t *testing.T) {
	settings := ArchiveSettings{}
	f := writeRecipeFile(t, `[[strategy]]
target_platforms = ["windows"]
compression = "none"
`)
	if err := settings.ReadFile(f); err != nil {
		t.Fatalf("ReadFile() failed: %v", err)
	}
	if _, err := settings.StrategyFor(PlatformLinux); err == nil {
		t.Error("StrategyFor() succeeded for a platform that no strategy targets")
	}
}

func TestSettingsInvalid(t *testing.T) {
	for _, tc := range []struct {
		name    string
		content string
		want    string
	}{
		{"no strategies", ``, "no [[strategy]]"},
		{"empty target platforms", `[[strategy]]
target_platforms = []
compression = "none"
`, "target_platforms is empty"},
		{"unknown platform", `[[strategy]]
target_platforms = ["emscripten"]
compression = "none"
`, `unknown platform "emscripten"`},
		{"duplicate platform", `[[strategy]]
target_platforms = ["windows", "linux"]
compression = "none"

[[strategy]]
target_platforms = ["linux"]
compression = "none"
`, `strategies 1 and 2 both target "linux"`},
		{"missing compression", `[[strategy]]
target_platforms = ["windows"]
`, "compression is required"},
		{"unknown compression", `[[strategy]]
target_platforms = ["windows"]
compression = "lz4"
`, `unknown compression "lz4"`},
		{"zstd without settings", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"
`, "[strategy.zstd] is missing"},
		{"missing min_saved_bytes", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
max_ratio_percent = 90
`, "min_saved_bytes is required"},
		{"zero min_saved_bytes", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 0
max_ratio_percent = 90
`, "min_saved_bytes must be at least 1"},
		{"missing max_ratio_percent", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 1024
`, "max_ratio_percent is required"},
		{"max_ratio_percent above 100", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 1024
max_ratio_percent = 100.5
`, "max_ratio_percent must be greater than 0 and at most 100"},
		{"missing level", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
min_saved_bytes = 1024
max_ratio_percent = 90
`, "level is required"},
		{"level above 22", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 23
min_saved_bytes = 1024
max_ratio_percent = 90
`, "level must be from 1 to 22, got 23"},
		{"dictionary without name", testZstdStrategy + `
[[strategy.zstd.dictionary]]
max_size = 4096
paths = ["**/*.json"]
`, "dictionary 1: name is required"},
		{"dictionary name with a slash", testZstdStrategy + `
[[strategy.zstd.dictionary]]
name = "a/b"
max_size = 4096
paths = ["**/*.json"]
`, `name "a/b" contains '/'`},
		{"dictionary without max_size", testZstdStrategy + `
[[strategy.zstd.dictionary]]
name = "text"
paths = ["**/*.json"]
`, "text: max_size is required"},
		{"small dictionary", testZstdStrategy + `
[[strategy.zstd.dictionary]]
name = "text"
max_size = 255
paths = ["**/*.json"]
`, "text: max_size must be at least 256, got 255"},
		{"dictionary without paths", testZstdStrategy + `
[[strategy.zstd.dictionary]]
name = "text"
max_size = 4096
`, "text: paths is empty"},
		{"malformed dictionary path", testZstdStrategy + `
[[strategy.zstd.dictionary]]
name = "text"
max_size = 4096
paths = ["shaders/[a.glsl"]
`, `path pattern "shaders/[a.glsl" is malformed`},
		{"duplicate dictionary name", testZstdStrategy + `
[[strategy.zstd.dictionary]]
name = "text"
max_size = 4096
paths = ["**/*.json"]

[[strategy.zstd.dictionary]]
name = "text"
max_size = 4096
paths = ["**/*.svg"]
`, `dictionary 2: duplicate name "text"`},
		{"misspelled key", `[[strategy]]
target_platforms = ["web"]
compression = "zstd"

[strategy.zstd]
level = 19
min_saved_bytes = 1024
max_ratio_precent = 90
`, "max_ratio_precent"},
	} {
		t.Run(tc.name, func(t *testing.T) {
			settings := ArchiveSettings{}
			err := settings.ReadFile(writeRecipeFile(t, tc.content))
			if err == nil {
				t.Fatal("ReadFile() succeeded, want an error")
			}
			if !strings.Contains(err.Error(), tc.want) {
				t.Errorf("error %q does not contain %q", err, tc.want)
			}
		})
	}
}

func TestSettingsTooManyDictionaries(t *testing.T) {
	var sb strings.Builder
	sb.WriteString(testZstdStrategy)
	for i := range ZstdMaxDictionaries + 1 {
		fmt.Fprintf(&sb, "\n[[strategy.zstd.dictionary]]\nname = \"d%d\"\nmax_size = 1024\npaths = [\"d%d/*\"]\n", i, i)
	}

	settings := ArchiveSettings{}
	err := settings.ReadFile(writeRecipeFile(t, sb.String()))
	if err == nil {
		t.Fatal("ReadFile() succeeded, want an error for too many dictionaries")
	}
	if want := "257 dictionaries, but a strategy can have at most 256"; !strings.Contains(err.Error(), want) {
		t.Errorf("error %q does not contain %q", err, want)
	}
}

func TestZstdSettingsAccepts(t *testing.T) {
	minSavedBytes := int64(1024)
	maxRatioPercent := 90.0
	s := &ZstdSettings{MinSavedBytes: &minSavedBytes, MaxRatioPercent: &maxRatioPercent}

	const KiB = 1024
	const MiB = 1024 * KiB
	for _, tc := range []struct {
		uncompressed int
		compressed   int
		want         bool
	}{
		{100, 60, false},                 // NOTE: 60% but saves only 40 bytes.
		{100 * MiB, 97 * MiB, false},     // NOTE: Saves 3 MiB but keeps 97%.
		{200 * KiB, 112 * KiB, true},     // NOTE: 56% and saves 88 KiB.
		{0, 9, false},                    // NOTE: An empty file grows into a frame header.
		{10 * KiB, 9 * KiB, true},        // NOTE: Exactly 90% and 1 KiB saved.
		{10 * KiB, 9*KiB + 1, false},     // NOTE: Saves one byte less than 1 KiB.
		{20 * KiB, 18*KiB + 1, false},    // NOTE: Saves enough but exceeds 90%.
		{64 * KiB, 64*KiB + 13, false},   // NOTE: Incompressible data grows.
		{4 * MiB, 3 * MiB, true},         // NOTE: 75% and saves 1 MiB.
		{2048, 1024, true},               // NOTE: 50% and saves exactly 1 KiB.
		{2047, 1024, false},              // NOTE: 50% but saves 1023 bytes.
		{12 * MiB, 4 * MiB, true},        // NOTE: Larger than the zstd window.
		{100 * KiB, 90*KiB + 512, false}, // NOTE: 90.5% exceeds 90%.
	} {
		got := s.Accepts(tc.uncompressed, tc.compressed)
		if got != tc.want {
			t.Errorf("Accepts(%d, %d) = %v, want %v", tc.uncompressed, tc.compressed, got, tc.want)
		}
	}
}
