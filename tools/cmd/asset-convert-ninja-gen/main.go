// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strconv"
	"strings"

	ninja "github.com/mogemimi/pomdog/tools/pkg/ninja"
	toml "github.com/pelletier/go-toml/v2"
)

type Env struct {
	InRecipe      string
	InDir         string
	ThirdPartyDir string
	OutNinjaFile  string
	OutDir        string
	IntDir        string
	ToolDir       string
	Verbose       bool
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InRecipe, "recipe", "", "convert recipe file (*.toml)")
	flag.StringVar(&env.InDir, "indir", "", "input asset directory")
	flag.StringVar(&env.ThirdPartyDir, "thirdpartydir", "", "third-party directory (replaces ${POMDOG_THIRDPARTY_DIR})")
	flag.StringVar(&env.OutNinjaFile, "outninja", "", "output ninja file (build.ninja)")
	flag.StringVar(&env.OutDir, "outdir", "", "output content directory")
	flag.StringVar(&env.IntDir, "intdir", "", "intermediate directory")
	flag.StringVar(&env.ToolDir, "tooldir", "", "tool directory")
	flag.BoolVar(&env.Verbose, "verbose", false, "verbose output")
	flag.Parse()

	if env.InRecipe == "" {
		log.Fatal("-recipe is required")
	}
	if env.OutNinjaFile == "" {
		log.Fatal("-outninja is required")
	}
	if env.OutDir == "" {
		log.Fatal("-outdir is required")
	}
	if env.ToolDir == "" {
		log.Fatal("-tooldir is required")
	}

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

// ConvertRecipe represents the TOML recipe for asset conversion.
type ConvertRecipe struct {
	GameControllerDB []*GameControllerDBEntry `toml:"game_controller_db"`
	SoundClip        []*SoundClipEntry        `toml:"sound_clip"`
}

// GameControllerDBEntry defines a conversion from SDL_GameControllerDB to .gcdb binary.
type GameControllerDBEntry struct {
	In              string `toml:"in"`
	Out             string `toml:"out"`
	UseAbsPathInDir bool   `toml:"use_abs_path_indir"`
}

// SoundClipEntry defines a single sound clip conversion.
type SoundClipEntry struct {
	InFile        string         `toml:"in_file"`
	OutFile       string         `toml:"out_file"`
	InFormat      string         `toml:"in_format"`
	OutFormat     string         `toml:"out_format"`
	WAVOptions    *WAVOptions    `toml:"wav_options"`
	VorbisOptions *VorbisOptions `toml:"vorbis_options"`
	Loop          *SoundClipLoop `toml:"loop"`
}

// WAVOptions specifies output options for WAV format.
type WAVOptions struct {
	Channels       int  `toml:"channels"`
	SampleRate     int  `toml:"sample_rate"`
	BitDepth       int  `toml:"bit_depth"`
	InvalidBitRate *int `toml:"bit_rate"` // NOTE: Invalid option to detect misconfiguration
}

// VorbisOptions specifies output options for Ogg Vorbis format.
type VorbisOptions struct {
	Channels        int  `toml:"channels"`
	SampleRate      int  `toml:"sample_rate"`
	BitRate         int  `toml:"bit_rate"`
	InvalidBitDepth *int `toml:"bit_depth"` // NOTE: Invalid option to detect misconfiguration
}

// SoundClipLoop defines crossfade loop parameters for a sound clip.
type SoundClipLoop struct {
	TrimStart    bool    `toml:"trim_start"`
	Seconds      float64 `toml:"seconds"`
	AlignSamples int     `toml:"align_samples"`
	FadeMethod   string  `toml:"fade_method"`
}

func run(env *Env) error {
	if abs, err := filepath.Abs(env.OutDir); err == nil {
		env.OutDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutNinjaFile); err == nil {
		env.OutNinjaFile = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.ToolDir); err == nil {
		env.ToolDir = filepath.Clean(abs)
	}
	if env.InDir != "" {
		if abs, err := filepath.Abs(env.InDir); err == nil {
			env.InDir = filepath.Clean(abs)
		}
	}
	if env.IntDir != "" {
		if abs, err := filepath.Abs(env.IntDir); err == nil {
			env.IntDir = filepath.Clean(abs)
		}
	}
	if env.ThirdPartyDir != "" {
		if abs, err := filepath.Abs(env.ThirdPartyDir); err == nil {
			env.ThirdPartyDir = filepath.Clean(abs)
		}
	}

	recipe, err := readRecipe(env.InRecipe, env.ThirdPartyDir)
	if err != nil {
		return fmt.Errorf("readRecipe() failed: %w", err)
	}

	gen := &ninja.Generator{}

	// NOTE: Game controller DB conversion rule
	gcdbExe := filepath.Join(env.ToolDir, "generate-game-controller-db")
	if runtime.GOOS == "windows" {
		gcdbExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("generate_game_controller_db_exe", gcdbExe))
	gen.AddRule(&ninja.Rule{
		Name:    "generate_game_controller_db",
		Command: "$generate_game_controller_db_exe -i $in -o $out",
	})

	// NOTE: Track output directories to create
	outDirs := map[string]struct{}{}

	for _, entry := range recipe.GameControllerDB {
		inPath := entry.In
		if entry.UseAbsPathInDir {
			// NOTE: Input path is already absolute (e.g. from thirdparty dir)
		} else if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		outPath := filepath.Join(env.OutDir, entry.Out)
		outDirs[filepath.Dir(outPath)] = struct{}{}

		gen.AddBuild(&ninja.Build{
			Rule:    "generate_game_controller_db",
			OutFile: outPath,
			InFiles: []string{inPath},
		})
	}

	// NOTE: Create output directories
	dirs := make([]string, 0, len(outDirs))
	for d := range outDirs {
		dirs = append(dirs, d)
	}
	sort.Strings(dirs)

	for _, d := range dirs {
		if err := os.MkdirAll(d, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	// NOTE: Sound clip conversion
	if err := buildSoundClips(env, recipe, gen); err != nil {
		return fmt.Errorf("buildSoundClips() failed: %w", err)
	}

	// NOTE: Create directory for ninja file
	if dir := filepath.Dir(env.OutNinjaFile); dir != "" {
		if err := os.MkdirAll(dir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	content := gen.Generate()

	f, err := os.Create(env.OutNinjaFile)
	if err != nil {
		return fmt.Errorf("os.Create() failed: %w", err)
	}
	defer f.Close()

	if _, err := f.WriteString(content); err != nil {
		return fmt.Errorf("f.WriteString() failed: %w", err)
	}

	return nil
}

func readRecipe(inFile string, thirdPartyDir string) (*ConvertRecipe, error) {
	buf, err := os.ReadFile(inFile)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	// NOTE: Substitute environment variables
	content := string(buf)
	if thirdPartyDir != "" {
		thirdPartyDir = filepath.ToSlash(filepath.Clean(thirdPartyDir))
		content = strings.ReplaceAll(content, "${POMDOG_THIRDPARTY_DIR}", thirdPartyDir)
	}

	recipe := &ConvertRecipe{}
	if err := toml.Unmarshal([]byte(content), recipe); err != nil {
		return nil, fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	if err := validateRecipe(recipe); err != nil {
		return nil, fmt.Errorf("validateRecipe() failed: %w", err)
	}

	return recipe, nil
}

func validateRecipe(recipe *ConvertRecipe) error {
	for i, entry := range recipe.GameControllerDB {
		if entry.In == "" {
			return fmt.Errorf("[[game_controller_db]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[game_controller_db]] #%d: out must not be empty", i)
		}
	}
	for i, clip := range recipe.SoundClip {
		if clip.InFile == "" {
			return fmt.Errorf("[[sound_clip]] #%d: in_file must not be empty", i)
		}
		if clip.OutFile == "" {
			return fmt.Errorf("[[sound_clip]] #%d: out_file must not be empty", i)
		}
		if clip.InFormat == "" {
			return fmt.Errorf("[[sound_clip]] #%d: in_format must not be empty", i)
		}
		if clip.OutFormat == "" {
			return fmt.Errorf("[[sound_clip]] #%d: out_format must not be empty", i)
		}
		if clip.OutFormat != "wav" && clip.OutFormat != "vorbis" {
			return fmt.Errorf("[[sound_clip]] #%d: out_format must be 'wav' or 'vorbis'", i)
		}
		if clip.Loop != nil {
			if clip.Loop.FadeMethod != "" {
				switch clip.Loop.FadeMethod {
				case "linear", "equal_power", "logarithmic":
				default:
					return fmt.Errorf("[[sound_clip]] #%d: unsupported fade_method '%s'", i, clip.Loop.FadeMethod)
				}
			}
		}
	}
	return nil
}

func buildSoundClips(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.SoundClip) == 0 {
		return nil
	}

	// NOTE: wav-crossfade-conv tool
	wavCrossfadeExe := filepath.Join(env.ToolDir, "wav-crossfade-conv")
	if runtime.GOOS == "windows" {
		wavCrossfadeExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("wav_crossfade_conv_exe", wavCrossfadeExe))
	gen.AddRule(&ninja.Rule{
		Name:    "wav_crossfade_conv",
		Command: "$wav_crossfade_conv_exe -i $in -o $out --trimstart=$trimstart --seconds=$seconds --alignsamples=$alignsamples --fade=$fade",
	})

	// NOTE: ffmpeg tool
	ffmpegExe := filepath.Join(env.ToolDir, "ffmpeg")
	if runtime.GOOS == "windows" {
		ffmpegExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("ffmpeg_exe", ffmpegExe))

	ffmpegGlobalArgs := ""
	if !env.Verbose {
		ffmpegGlobalArgs = "-hide_banner -loglevel error"
	}
	gen.AddRule(&ninja.Rule{
		Name:    "ffmpeg_wav_to_wav",
		Command: fmt.Sprintf("$ffmpeg_exe %s -i $in -y -map_metadata -1 -f wav -bitexact $ffmpeg_args $out", ffmpegGlobalArgs),
	})
	gen.AddRule(&ninja.Rule{
		Name:    "ffmpeg_wav_to_vorbis",
		Command: fmt.Sprintf("$ffmpeg_exe %s -i $in -y -map_metadata -1 -f ogg -c:a libvorbis $ffmpeg_args $out", ffmpegGlobalArgs),
	})

	for _, clip := range recipe.SoundClip {
		inFile := clip.InFile
		if env.InDir != "" {
			inFile = filepath.Join(env.InDir, inFile)
		}

		outFile := filepath.Join(env.OutDir, clip.OutFile)

		if err := os.MkdirAll(filepath.Dir(outFile), os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}

		currentIn := inFile

		// Step 1: Crossfade loop (if loop config is specified)
		if clip.Loop != nil {
			if clip.InFormat != "wav" {
				return fmt.Errorf("crossfade loop is only supported for wav format: %s", clip.InFormat)
			}

			// NOTE: Use intermediate directory for loop output
			base := strings.TrimSuffix(filepath.Base(outFile), filepath.Ext(outFile))
			loopOutFile := filepath.Join(env.IntDir, base+"-loop.wav")

			if err := os.MkdirAll(filepath.Dir(loopOutFile), os.ModePerm); err != nil {
				return fmt.Errorf("os.MkdirAll() failed: %w", err)
			}

			fadeMethod := clip.Loop.FadeMethod
			if fadeMethod == "" {
				fadeMethod = "equal_power"
			}

			gen.AddBuild(&ninja.Build{
				Rule:    "wav_crossfade_conv",
				OutFile: loopOutFile,
				InFiles: []string{inFile},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("trimstart", strconv.FormatBool(clip.Loop.TrimStart)),
					ninja.NewVariableAsString("seconds", strconv.FormatFloat(clip.Loop.Seconds, 'f', -1, 64)),
					ninja.NewVariableAsString("alignsamples", strconv.Itoa(clip.Loop.AlignSamples)),
					ninja.NewVariableAsString("fade", fadeMethod),
				},
			})

			currentIn = loopOutFile
		}

		// Step 2: Format conversion with ffmpeg
		if clip.InFormat == "wav" && clip.OutFormat == "vorbis" {
			if clip.WAVOptions != nil {
				return fmt.Errorf("wav_options is not supported when output format is vorbis")
			}

			ffmpegArgs := ""
			if o := clip.VorbisOptions; o != nil {
				if o.InvalidBitDepth != nil {
					return fmt.Errorf("bit_depth option is not supported for vorbis output")
				}
				if o.SampleRate == 22050 && o.BitRate >= 128 {
					return fmt.Errorf("bit_rate must be less than 128 kbps when sample_rate is 22050 Hz for vorbis output")
				}

				switch o.Channels {
				case 1, 2:
					ffmpegArgs += " -ac " + strconv.Itoa(o.Channels)
				default:
					return fmt.Errorf("not supported channels: %d", o.Channels)
				}

				switch o.SampleRate {
				case 16000, 22050, 32000, 44100, 48000:
					ffmpegArgs += " -ar " + strconv.Itoa(o.SampleRate)
				default:
					return fmt.Errorf("not supported sample-rate: %d", o.SampleRate)
				}

				switch o.BitRate {
				case 64, 96, 128, 192, 256, 320:
					ffmpegArgs += " -b:a " + strconv.Itoa(o.BitRate) + "k"
				default:
					return fmt.Errorf("not supported bit-rate: %d kbps", o.BitRate)
				}
			}
			ffmpegArgs = strings.TrimSpace(ffmpegArgs)

			gen.AddBuild(&ninja.Build{
				Rule:    "ffmpeg_wav_to_vorbis",
				OutFile: outFile,
				InFiles: []string{currentIn},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("ffmpeg_args", ffmpegArgs),
				},
			})
		} else if clip.InFormat == "wav" && clip.OutFormat == "wav" {
			if clip.VorbisOptions != nil {
				return fmt.Errorf("vorbis_options is not supported when output format is wav")
			}

			ffmpegArgs := ""
			if o := clip.WAVOptions; o != nil {
				if o.InvalidBitRate != nil {
					return fmt.Errorf("bit_rate option is not supported for wav output")
				}

				switch o.Channels {
				case 1, 2:
					ffmpegArgs += " -ac " + strconv.Itoa(o.Channels)
				default:
					return fmt.Errorf("not supported channels: %d", o.Channels)
				}

				switch o.SampleRate {
				case 16000, 22050, 32000, 44100, 48000:
					ffmpegArgs += " -ar " + strconv.Itoa(o.SampleRate)
				default:
					return fmt.Errorf("not supported sample-rate: %d", o.SampleRate)
				}

				switch o.BitDepth {
				case 16:
					ffmpegArgs += " -c:a pcm_s16le"
				case 24:
					ffmpegArgs += " -c:a pcm_s24le"
				case 32:
					ffmpegArgs += " -c:a pcm_f32le"
				default:
					return fmt.Errorf("not supported bit-depth: %d", o.BitDepth)
				}
			}
			ffmpegArgs = strings.TrimSpace(ffmpegArgs)

			gen.AddBuild(&ninja.Build{
				Rule:    "ffmpeg_wav_to_wav",
				OutFile: outFile,
				InFiles: []string{currentIn},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("ffmpeg_args", ffmpegArgs),
				},
			})
		} else {
			return fmt.Errorf("unsupported output format: %s to %s", clip.InFormat, clip.OutFormat)
		}
	}

	return nil
}
