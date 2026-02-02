// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"context"
	"crypto/md5"
	"crypto/sha256"
	"encoding/hex"
	"errors"
	"fmt"
	"io"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"slices"
	"strings"

	"golang.org/x/sync/errgroup"
)

type RunArgs struct {
	BuildDir       string
	PomdogDir      string
	CMakeGenerator string
	SkipCppBuild   bool
	SkipDownload   bool
}

func run(config *Config, args *RunArgs) error {
	toolsDir := filepath.Join(args.BuildDir, "tools")

	// NOTE: make directories
	if err := command([]string{"mkdir", "-p", toolsDir}); err != nil {
		return err
	}

	if !args.SkipDownload {
		downloadDir := filepath.Join(args.BuildDir, "downloads")

		// NOTE: make directories
		if err := command([]string{"mkdir", "-p", downloadDir}); err != nil {
			return err
		}

		for _, src := range config.Downloads {
			currentPlatform := getPlatformString()
			if src.Platform != currentPlatform {
				continue
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

			if len(src.MD5) > 0 {
				if err := verifyMD5(temp, src.MD5); err != nil {
					return fmt.Errorf("MD5 verification failed for '%s': %w", src.URL, err)
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

	toolsBuildDir := filepath.Join(args.BuildDir, "thirdparty_builds")

	// NOTE: make directories
	if err := command([]string{"mkdir", "-p", toolsBuildDir}); err != nil {
		return err
	}

	const defaultCMakeGeneratorMSVS = "Visual Studio 18"

	// Determine CMake generator
	cmakeGenerator := args.CMakeGenerator
	if cmakeGenerator == "" {
		switch runtime.GOOS {
		case "windows":
			cmakeGenerator = defaultCMakeGeneratorMSVS
		case "darwin":
			cmakeGenerator = "Xcode"
		case "linux":
			cmakeGenerator = "Ninja"
		}
	}

	// Determine executable suffix
	exeSuffix := ""
	if runtime.GOOS == "windows" {
		exeSuffix = ".exe"
	}

	// Expand environment variables in BuildCpp configurations
	config.ExpandEnvBuildCpp(".", args.BuildDir, args.PomdogDir, cmakeGenerator, exeSuffix)

	if !args.SkipCppBuild {
		// NOTE: Build C++ tools from toml configuration
		currentPlatform := getPlatformString()
		processedTools := make(map[string]bool)

		for _, build := range config.BuildCpp {
			// Skip if already processed this tool
			if processedTools[build.Name] {
				continue
			}

			// Check if current platform matches
			if !slices.Contains(build.Platforms, currentPlatform) {
				continue
			}

			processedTools[build.Name] = true

			fmt.Printf("Building %s...\n", build.Name)

			// Execute commands
			for _, cmdStr := range build.Commands {
				args := parseCommand(cmdStr)
				if len(args) == 0 {
					continue
				}
				if err := command(args); err != nil {
					return fmt.Errorf("failed to build %s: %w", build.Name, err)
				}
			}

			// Copy output file to tools directory
			outFile := filepath.Join(toolsDir, build.Name+exeSuffix)
			if err := command([]string{"cp", build.OutFile, outFile}); err != nil {
				return fmt.Errorf("failed to copy %s: %w", build.Name, err)
			}
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

func verifyMD5(filePath string, expectedHash string) error {
	f, err := os.Open(filePath)
	if err != nil {
		return fmt.Errorf("failed to open file: %w", err)
	}
	defer f.Close()

	h := md5.New()
	if _, err := io.Copy(h, f); err != nil {
		return fmt.Errorf("failed to read file: %w", err)
	}

	actualHash := hex.EncodeToString(h.Sum(nil))
	if actualHash != expectedHash {
		return fmt.Errorf("hash mismatch: expected %s, got %s", expectedHash, actualHash)
	}

	return nil
}

// getPlatformString returns a platform identifier string combining OS and architecture.
// Examples: "windows_amd64", "windows_arm64", "mac_amd64", "mac_arm64", "linux_amd64", "linux_arm64"
func getPlatformString() string {
	os := runtime.GOOS
	arch := runtime.GOARCH

	// Normalize OS name
	switch os {
	case "darwin":
		os = "mac"
	}

	return os + "_" + arch
}

// parseCommand parses a command string into arguments.
// It handles spaces within quotes and basic shell-like argument splitting.
func parseCommand(cmdStr string) []string {
	var args []string
	var current strings.Builder
	inQuote := false
	quoteChar := rune(0)

	for _, r := range cmdStr {
		switch {
		case r == '"' || r == '\'':
			if inQuote && r == quoteChar {
				inQuote = false
				quoteChar = 0
			} else if !inQuote {
				inQuote = true
				quoteChar = r
			} else {
				current.WriteRune(r)
			}
		case r == ' ' || r == '\t':
			if inQuote {
				current.WriteRune(r)
			} else if current.Len() > 0 {
				args = append(args, current.String())
				current.Reset()
			}
		default:
			current.WriteRune(r)
		}
	}

	if current.Len() > 0 {
		args = append(args, current.String())
	}

	return args
}
