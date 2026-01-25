// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"context"
	"crypto/sha256"
	"encoding/hex"
	"errors"
	"fmt"
	"io"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"

	"golang.org/x/sync/errgroup"
)

type RunArgs struct {
	BuildDir     string
	PomdogDir    string
	SkipCppBuild bool
	SkipDownload bool
}

func run(config *Config, args *RunArgs) error {
	toolsDir := filepath.Join(args.BuildDir, "tools")

	// NOTE: make directories
	if err := command([]string{"mkdir", "-p", toolsDir}); err != nil {
		return err
	}

	toolsBuildDir := filepath.Join(args.BuildDir, "thirdparty_builds")

	// NOTE: make directories
	if err := command([]string{"mkdir", "-p", toolsBuildDir}); err != nil {
		return err
	}

	const cmakeTargetMSVS = "Visual Studio 18"

	if !args.SkipCppBuild {
		sourceNinjaDir := filepath.ToSlash(filepath.Clean(filepath.Join(args.PomdogDir, "thirdparty", "ninja")))
		buildNinjaDir := filepath.ToSlash(filepath.Clean(filepath.Join(toolsBuildDir, "ninja")))

		// NOTE: Compile ninja
		switch targetOS := runtime.GOOS; targetOS {
		case "windows":
			if err := command([]string{"cmake", "-B" + buildNinjaDir, "-H" + sourceNinjaDir, "-G", cmakeTargetMSVS}); err != nil {
				return err
			}
			if err := command([]string{"cmake", `--build`, buildNinjaDir, `--config`, "Release"}); err != nil {
				return err
			}
		case "darwin":
			if err := command([]string{"cmake", "-B" + buildNinjaDir, "-H" + sourceNinjaDir, "-G", "Xcode"}); err != nil {
				return err
			}
			if err := command([]string{"xcodebuild", "-project", filepath.Join(buildNinjaDir, "ninja.xcodeproj"), "-configuration", "Release"}); err != nil {
				return err
			}
		}

		if err := command([]string{"cp", filepath.Join(buildNinjaDir, "Release", "ninja"), filepath.Join(toolsDir, "ninja")}); err != nil {
			return err
		}
	}

	if !args.SkipDownload {
		downloadDir := filepath.Join(args.BuildDir, "downloads")

		// NOTE: make directories
		if err := command([]string{"mkdir", "-p", downloadDir}); err != nil {
			return err
		}

		for _, src := range config.Downloads {
			switch targetOS := runtime.GOOS; targetOS {
			case "windows":
				if src.Platform != "windows" {
					continue
				}
			case "darwin":
				if src.Platform != "mac" {
					continue
				}
			}

			outFile := filepath.Join(toolsDir, src.OutFile)
			if _, err := os.Stat(outFile); err == nil {
				// NOTE: outFile exists
				continue
			} else if !errors.Is(err, os.ErrNotExist) {
				return fmt.Errorf("os.Stat() failed '%s': %w", outFile, err)
			}

			tempDir, err := os.MkdirTemp(downloadDir, "dl_")
			if err != nil {
				return fmt.Errorf("os.MkdirTemp() failed '%s': %w", downloadDir, err)
			}

			temp := filepath.Join(tempDir, filepath.Base(src.URL))
			if err := command([]string{"curl", src.URL, "-L", "-o", temp}); err != nil {
				return err
			}

			if len(src.SHA256) > 0 {
				if err := verifySHA256(temp, src.SHA256); err != nil {
					return fmt.Errorf("SHA256 verification failed for '%s': %w", src.URL, err)
				}
			}

			if err := command([]string{"unzip", temp, "-d", tempDir}); err != nil {
				return err
			}

			if len(src.InFile) == 0 {
				if err := command([]string{"mv", tempDir, outFile}); err != nil {
					return err
				}
			} else {
				if err := command([]string{"cp", filepath.Join(tempDir, src.InFile), outFile}); err != nil {
					return err
				}
			}
		}
	}

	if !args.SkipCppBuild {
		sourceSPIRVCrossDir := filepath.ToSlash(filepath.Clean(filepath.Join(args.PomdogDir, "thirdparty", "spirv-cross")))
		buildSPIRVCrossDir := filepath.ToSlash(filepath.Clean(filepath.Join(toolsBuildDir, "spirv-cross")))
		cmakeOptions := []string{"-DSPIRV_CROSS_ENABLE_TESTS=OFF"}

		// NOTE: Compile spirv-cross
		switch targetOS := runtime.GOOS; targetOS {
		case "windows":
			if err := command(append([]string{"cmake", "-B" + buildSPIRVCrossDir, "-H" + sourceSPIRVCrossDir, "-G", cmakeTargetMSVS}, cmakeOptions...)); err != nil {
				return err
			}
			if err := command([]string{"cmake", `--build`, buildSPIRVCrossDir, `--config`, "Release"}); err != nil {
				return err
			}
		case "darwin":
			if err := command(append([]string{"cmake", "-B" + buildSPIRVCrossDir, "-H" + sourceSPIRVCrossDir, "-G", "Xcode"}, cmakeOptions...)); err != nil {
				return err
			}
			if err := command([]string{"xcodebuild", "-project", filepath.Join(buildSPIRVCrossDir, "SPIRV-Cross.xcodeproj"), "-configuration", "Release"}); err != nil {
				return err
			}
		}

		if err := command([]string{"cp", filepath.Join(buildSPIRVCrossDir, "Release", "spirv-cross"), filepath.Join(toolsDir, "spirv-cross")}); err != nil {
			return err
		}
	}

	if !args.SkipCppBuild {
		sourceGLSLangDir := filepath.ToSlash(filepath.Clean(filepath.Join(args.PomdogDir, "thirdparty", "glslang")))
		buildSGLSLangDir := filepath.ToSlash(filepath.Clean(filepath.Join(toolsBuildDir, "glslang")))
		cmakeOptions := []string{"-DBUILD_EXTERNAL=OFF", "-DENABLE_CTEST=OFF", "-DENABLE_OPT=0"}

		// NOTE: Compile glslang
		switch targetOS := runtime.GOOS; targetOS {
		case "windows":
			pythonExe := filepath.Join(toolsDir, "python", "python.exe")
			if abs, err := filepath.Abs(pythonExe); err == nil {
				pythonExe = abs
			}

			cmakeOptions = append(cmakeOptions, "-DPython3_EXECUTABLE="+pythonExe)

			if err := command(append([]string{"cmake", "-B" + buildSGLSLangDir, "-H" + sourceGLSLangDir, "-G", cmakeTargetMSVS}, cmakeOptions...)); err != nil {
				return err
			}
			if err := command([]string{"cmake", `--build`, buildSGLSLangDir, `--config`, "Release"}); err != nil {
				return err
			}
		case "darwin":
			if err := command(append([]string{"cmake", "-B" + buildSGLSLangDir, "-H" + sourceGLSLangDir, "-G", "Xcode"}, cmakeOptions...)); err != nil {
				return err
			}
			if err := command([]string{"xcodebuild", "-project", filepath.Join(buildSGLSLangDir, "glslang.xcodeproj"), "-configuration", "Release"}); err != nil {
				return err
			}
		}

		if err := command([]string{"cp", filepath.Join(buildSGLSLangDir, "StandAlone", "Release", "glslangValidator"), filepath.Join(toolsDir, "glslangValidator")}); err != nil {
			return err
		}
	}

	if !args.SkipCppBuild {
		sourceFlatBuffersDir := filepath.ToSlash(filepath.Clean(filepath.Join(args.PomdogDir, "thirdparty", "flatbuffers")))
		buildFlatBuffersDir := filepath.ToSlash(filepath.Clean(filepath.Join(toolsBuildDir, "flatbuffers")))
		cmakeOptions := []string{"-DFLATBUFFERS_BUILD_TESTS=OFF"}

		// NOTE: Compile flatc
		switch targetOS := runtime.GOOS; targetOS {
		case "windows":
			if err := command(append([]string{"cmake", "-B" + buildFlatBuffersDir, "-H" + sourceFlatBuffersDir, "-G", cmakeTargetMSVS}, cmakeOptions...)); err != nil {
				return err
			}
			if err := command([]string{"cmake", `--build`, buildFlatBuffersDir, `--config`, "Release"}); err != nil {
				return err
			}
		case "darwin":
			if err := command(append([]string{"cmake", "-B" + buildFlatBuffersDir, "-H" + sourceFlatBuffersDir, "-G", "Xcode"}, cmakeOptions...)); err != nil {
				return err
			}
			if err := command([]string{"xcodebuild", "-project", filepath.Join(buildFlatBuffersDir, "FlatBuffers.xcodeproj"), "-configuration", "Release"}); err != nil {
				return err
			}
		}

		if err := command([]string{"cp", filepath.Join(buildFlatBuffersDir, "Release", "flatc"), filepath.Join(toolsDir, "flatc")}); err != nil {
			return err
		}
	}

	{
		// NOTE: Build go tools
		ctx := context.Background()
		eg, _ := errgroup.WithContext(ctx)
		eg.SetLimit(6)

		for _, toolConf := range config.GoTools {
			if !toolConf.BuildEarlyPass {
				continue
			}

			for _, name := range toolConf.Tools {
				inDir := filepath.Clean(filepath.Join(toolConf.GoDir, name))
				outPath := filepath.Join(toolsDir, name)
				switch targetOS := runtime.GOOS; targetOS {
				case "windows":
					outPath = outPath + ".exe"
				}

				eg.Go(func() error {
					if !filepath.IsAbs(outPath) {
						if p, err := filepath.Abs(outPath); err != nil {
							return fmt.Errorf("filepath.Abs(): %s: %w", outPath, err)
						} else {
							outPath = p
						}
					}

					cmd := exec.Command("go", "build", "-o", outPath)
					cmd.Stdout = os.Stdout
					cmd.Stderr = os.Stderr
					cmd.Dir = inDir

					if err := cmd.Run(); err != nil {
						return fmt.Errorf("command: %s: %w", strings.Join(cmd.Args, " "), err)
					}
					return nil
				})
			}
		}

		if err := eg.Wait(); err != nil {
			return fmt.Errorf("failed to build go-tools: %w", err)
		}
	}

	// NOTE: Generate schemas
	for _, schemaConf := range config.Schemas {
		for _, schema := range schemaConf.Sources {
			flatc := filepath.Join(toolsDir, "flatc")
			{
				cmd := exec.Command(flatc, "--go", "-o", schemaConf.GoDir, "--gen-object-api", schema)
				cmd.Stdout = os.Stdout
				cmd.Stderr = os.Stderr
				if err := cmd.Run(); err != nil {
					return fmt.Errorf("command: %s: %w", strings.Join(cmd.Args, " "), err)
				}
			}
			{
				cmd := exec.Command(flatc, "--cpp", "-o", schemaConf.CppDir, "--gen-object-api", schema)
				cmd.Stdout = os.Stdout
				cmd.Stderr = os.Stderr
				if err := cmd.Run(); err != nil {
					return fmt.Errorf("command: %s: %w", strings.Join(cmd.Args, " "), err)
				}
			}
		}

		for _, modify := range schemaConf.GoModify {
			goSrc := filepath.Join(schemaConf.GoModDir, modify.Source)
			modifyExe := filepath.Join(toolsDir, "flatbuffers-go-modify")

			options := []string{"-o", goSrc, "-i", goSrc}
			if modify.SharedString {
				options = append(options, "--sharedstring")
			}

			cmd := exec.Command(modifyExe, options...)
			cmd.Stdout = os.Stdout
			cmd.Stderr = os.Stderr
			if err := cmd.Run(); err != nil {
				return fmt.Errorf("command: %s: %w", strings.Join(cmd.Args, " "), err)
			}
		}

		{
			modFile := filepath.Join(schemaConf.GoModDir, "go.mod")
			cmd := exec.Command("touch", modFile)
			cmd.Stdout = os.Stdout
			cmd.Stderr = os.Stderr
			if err := cmd.Run(); err != nil {
				return fmt.Errorf("command: %s: %w", strings.Join(cmd.Args, " "), err)
			}
		}
	}

	{
		// NOTE: Build go tools
		ctx := context.Background()
		eg, _ := errgroup.WithContext(ctx)
		eg.SetLimit(6)

		for _, toolConf := range config.GoTools {
			if toolConf.BuildEarlyPass {
				continue
			}

			for _, name := range toolConf.Tools {
				inDir := filepath.Clean(filepath.Join(toolConf.GoDir, name))
				outPath := filepath.Join(toolsDir, name)
				switch targetOS := runtime.GOOS; targetOS {
				case "windows":
					outPath = outPath + ".exe"
				}

				eg.Go(func() error {
					if !filepath.IsAbs(outPath) {
						if p, err := filepath.Abs(outPath); err != nil {
							return fmt.Errorf("filepath.Abs(): %s: %w", outPath, err)
						} else {
							outPath = p
						}
					}

					cmd := exec.Command("go", "build", "-o", outPath)
					cmd.Stdout = os.Stdout
					cmd.Stderr = os.Stderr
					cmd.Dir = inDir

					if err := cmd.Run(); err != nil {
						return fmt.Errorf("command: %s: %w", strings.Join(cmd.Args, " "), err)
					}
					return nil
				})
			}
		}

		if err := eg.Wait(); err != nil {
			return fmt.Errorf("failed to build go-tools: %w", err)
		}
	}

	return nil
}

func command(args []string) error {
	cmd := exec.Command(args[0], args[1:]...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Run(); err != nil {
		return fmt.Errorf("command: %s: %w", strings.Join(args, " "), err)
	}
	return nil
}

func verifySHA256(filePath string, expectedHash string) error {
	f, err := os.Open(filePath)
	if err != nil {
		return fmt.Errorf("failed to open file: %w", err)
	}
	defer f.Close()

	h := sha256.New()
	if _, err := io.Copy(h, f); err != nil {
		return fmt.Errorf("failed to read file: %w", err)
	}

	actualHash := hex.EncodeToString(h.Sum(nil))
	if actualHash != expectedHash {
		return fmt.Errorf("hash mismatch: expected %s, got %s", expectedHash, actualHash)
	}

	return nil
}
