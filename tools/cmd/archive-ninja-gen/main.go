// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"errors"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"

	archives "github.com/mogemimi/pomdog/tools/pkg/archives"
	ninja "github.com/mogemimi/pomdog/tools/pkg/ninja"
)

type Env struct {
	OutNinjaFile string
	ContentDir   string
	OutDir       string
	IntDir       string
	ToolDir      string
	SettingsFile string   // archive settings passed to archive-content
	Recipes      []string // positional args: archive*.toml files
}

func main() {
	env := &Env{}
	flag.StringVar(&env.OutNinjaFile, "outninja", "", "output ninja file (build.ninja)")
	flag.StringVar(&env.ContentDir, "contentdir", "", "content directory")
	flag.StringVar(&env.OutDir, "outdir", "", "output shipping directory")
	flag.StringVar(&env.IntDir, "intdir", "", "intermediate directory")
	flag.StringVar(&env.ToolDir, "tooldir", "", "tool directory")
	flag.StringVar(&env.SettingsFile, "settings", "", "archive settings file (*.toml)")
	flag.Parse()

	env.Recipes = flag.Args()

	if env.OutNinjaFile == "" {
		log.Fatal("-outninja is required")
	}
	if env.ContentDir == "" {
		log.Fatal("-contentdir is required")
	}
	if env.OutDir == "" {
		log.Fatal("-outdir is required")
	}
	if env.IntDir == "" {
		log.Fatal("-intdir is required")
	}
	if env.ToolDir == "" {
		log.Fatal("-tooldir is required")
	}
	if env.SettingsFile == "" {
		log.Fatal("-settings is required")
	}
	if len(env.Recipes) == 0 {
		log.Fatal("at least one archive recipe file (*.toml) must be specified as a positional argument")
	}

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

// writeFileIfChanged writes content to file unless the file already holds it,
// so that the modification time changes only with the content.
func writeFileIfChanged(file, content string) error {
	current, err := os.ReadFile(file)
	if err == nil && string(current) == content {
		return nil
	}
	if err != nil && !errors.Is(err, os.ErrNotExist) {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	if err := os.MkdirAll(filepath.Dir(file), os.ModePerm); err != nil {
		return fmt.Errorf("os.MkdirAll() failed: %w", err)
	}
	if err := os.WriteFile(file, []byte(content), 0o644); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}
	return nil
}

// zstdLevelFlags returns the zstd command-line flags for level.
func zstdLevelFlags(level int) string {
	// NOTE: Levels above 19 need --ultra, because they use more memory to compress.
	if level > 19 {
		return fmt.Sprintf("--ultra -%d", level)
	}
	return fmt.Sprintf("-%d", level)
}

func run(env *Env) error {
	if abs, err := filepath.Abs(env.ContentDir); err == nil {
		env.ContentDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutDir); err == nil {
		env.OutDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.IntDir); err == nil {
		env.IntDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutNinjaFile); err == nil {
		env.OutNinjaFile = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.ToolDir); err == nil {
		env.ToolDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.SettingsFile); err == nil {
		env.SettingsFile = filepath.Clean(abs)
	}

	absRecipes := make([]string, 0, len(env.Recipes))
	for _, r := range env.Recipes {
		if abs, err := filepath.Abs(r); err == nil {
			absRecipes = append(absRecipes, filepath.Clean(abs))
		} else {
			absRecipes = append(absRecipes, r)
		}
	}
	env.Recipes = absRecipes

	// NOTE: Read the settings and recipes here to list every file that each
	// archive packs, so the build compresses each file in its own build edge.
	settings := archives.ArchiveSettings{}
	if err := settings.ReadFile(env.SettingsFile); err != nil {
		return err
	}
	recipe := archives.ArchiveRecipe{}
	if err := recipe.ReadFiles(env.Recipes); err != nil {
		return err
	}

	platforms := []string{}
	switch runtime.GOOS {
	case "windows":
		platforms = append(platforms, archives.PlatformWindows, archives.PlatformLinux)
	case "darwin":
		platforms = append(platforms, archives.PlatformMacOS, archives.PlatformLinux)
	case "linux":
		platforms = append(platforms, archives.PlatformLinux)
	}
	platforms = append(platforms, archives.PlatformWeb)

	plans := make([]*archives.ArchivePlan, 0, len(platforms))
	usesZstd := false
	for _, platform := range platforms {
		plan, err := archives.PlanArchive(&settings, &recipe, []string{platform})
		if err != nil {
			return fmt.Errorf("%s: the %s archive: %w", env.SettingsFile, platform, err)
		}
		for _, file := range plan.Files {
			if file.Compression == archives.CompressionZstd {
				usesZstd = true
			}
		}
		plans = append(plans, plan)
	}

	gen := &ninja.Generator{}

	archiveExe := filepath.Join(env.ToolDir, "archive-content")
	zstdExe := filepath.Join(env.ToolDir, "zstd")
	zstdWrapperExe := filepath.Join(env.ToolDir, "zstd-wrapper")
	if runtime.GOOS == "windows" {
		archiveExe += ".exe"
		zstdExe += ".exe"
		zstdWrapperExe += ".exe"
	}

	gen.AddVariable(ninja.NewVariableAsPath("archive_content_exe", archiveExe))
	gen.AddVariable(ninja.NewVariableAsPath("archive_settings", env.SettingsFile))

	gen.AddRule(&ninja.Rule{
		Name:    "archive_content",
		Command: "$archive_content_exe --platform $platform --contentdir $contentdir --settings $archive_settings --zstddir $zstddir -o $out --outbin $outbin --outdebug $outdebug $in",
	})

	if usesZstd {
		gen.AddVariable(ninja.NewVariableAsPath("zstd_exe", zstdExe))
		gen.AddVariable(ninja.NewVariableAsPath("zstd_wrapper_exe", zstdWrapperExe))

		// NOTE: zstd gives each frame the modification time of its file, so the
		// frame would always look older than its inputs. zstd-wrapper runs zstd
		// and gives the output the time when zstd wrote it.
		gen.AddRule(&ninja.Rule{
			Name:    "zstd_compress",
			Command: "$zstd_wrapper_exe $zstd_exe -q -f $zstd_level $in -o $out",
		})
		gen.AddRule(&ninja.Rule{
			Name:    "zstd_compress_dictionary",
			Command: "$zstd_wrapper_exe $zstd_exe -q -f $zstd_level -D $dictionary $in -o $out",
		})

		// NOTE: Training with one thread and a fixed dictionary ID makes the
		// dictionary depend only on its files and settings.
		gen.AddRule(&ninja.Rule{
			Name:    "zstd_train",
			Command: "$zstd_wrapper_exe $zstd_exe --train -q -T1 $zstd_level --maxdict=$max_size --dictID=$dictionary_id --filelist=$filelist -o $out",
		})
	}

	contentPath := func(file string) string {
		return filepath.Join(env.ContentDir, filepath.FromSlash(file))
	}

	for i, platform := range platforms {
		plan := plans[i]

		outDir := filepath.Join(env.OutDir, platform)
		intDir := filepath.Join(env.IntDir, platform)
		zstdDir := filepath.Join(intDir, "zstd")

		zstdLevel := ""
		if plan.Strategy.Zstd != nil {
			zstdLevel = zstdLevelFlags(*plan.Strategy.Zstd.Level)
		}

		// NOTE: The archive depends on every file that archive-content reads,
		// so Ninja builds them first and rebuilds the archive when one changes.
		// The tool executables are inputs too, so that updating a tool rebuilds
		// the data that it produces. zstd-wrapper only sets the modification
		// time of its output, so the data does not depend on it.
		archiveInputs := []string{env.SettingsFile, archiveExe}

		for _, d := range plan.Dictionaries {
			out := archives.ZstdDictionaryPath(zstdDir, d.ID)

			inputs := make([]string, 0, len(d.Files))
			for _, file := range d.Files {
				inputs = append(inputs, contentPath(file))
			}

			// NOTE: zstd reads the training files from a list with one path per
			// line. A Ninja response file would end lines with CRLF on Windows,
			// which zstd reads as part of the paths, so the list is written here.
			// Keeping an unchanged list keeps the dictionary up to date.
			fileList := filepath.Join(zstdDir, "dictionaries", strconv.FormatUint(uint64(d.ID), 10)+".filelist")
			var sb strings.Builder
			for _, input := range inputs {
				sb.WriteString(filepath.ToSlash(input))
				sb.WriteString("\n")
			}
			if err := writeFileIfChanged(fileList, sb.String()); err != nil {
				return err
			}

			gen.AddBuild(&ninja.Build{
				Rule:            "zstd_train",
				OutFile:         out,
				InFiles:         inputs,
				ImplicitInFiles: []string{fileList, zstdExe},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("zstd_level", zstdLevel),
					ninja.NewVariableAsString("max_size", strconv.Itoa(*d.Settings.MaxSize)),
					ninja.NewVariableAsString("dictionary_id", strconv.FormatUint(uint64(d.ID), 10)),
					ninja.NewVariableAsPath("filelist", fileList),
				},
			})
			archiveInputs = append(archiveInputs, out)
		}

		for _, file := range plan.Files {
			archiveInputs = append(archiveInputs, contentPath(file.Path))
			if file.Compression != archives.CompressionZstd {
				continue
			}

			out := archives.ZstdFramePath(zstdDir, file.Path)
			build := &ninja.Build{
				Rule:            "zstd_compress",
				OutFile:         out,
				InFiles:         []string{contentPath(file.Path)},
				ImplicitInFiles: []string{zstdExe},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("zstd_level", zstdLevel),
				},
			}
			if file.Dictionary != nil {
				dictionary := archives.ZstdDictionaryPath(zstdDir, file.Dictionary.ID)
				build.Rule = "zstd_compress_dictionary"
				build.ImplicitInFiles = append(build.ImplicitInFiles, dictionary)
				build.Variables = append(build.Variables, ninja.NewVariableAsPath("dictionary", dictionary))
			}
			gen.AddBuild(build)
			archiveInputs = append(archiveInputs, out)
		}

		outIdx := filepath.Join(outDir, "content.idx")
		outPak := filepath.Join(outDir, "content.pak")
		outDebug := filepath.Join(intDir, "content.idx-debug")

		gen.AddBuild(&ninja.Build{
			Rule:             "archive_content",
			OutFile:          outIdx,
			ImplicitOutFiles: []string{outPak, outDebug},
			InFiles:          env.Recipes,
			ImplicitInFiles:  archiveInputs,
			Variables: []*ninja.Variable{
				ninja.NewVariableAsString("platform", platform),
				ninja.NewVariableAsPath("contentdir", env.ContentDir),
				ninja.NewVariableAsPath("zstddir", zstdDir),
				ninja.NewVariableAsPath("outbin", outPak),
				ninja.NewVariableAsPath("outdebug", outDebug),
			},
		})
	}

	// Create directory for ninja file
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
