// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"context"
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/fatih/color"
	"golang.org/x/sync/errgroup"
	"golang.org/x/sync/semaphore"
)

type projectConfig struct {
	Dir       string // Parent directory for the new project
	Name      string // Project name (lowercase alphanumeric + underscore)
	URL       string // Bundle identifier (e.g. com.example.my-game)
	PomdogDir string // Local pomdog directory (for template files)
	PomdogURL string // Git URL for pomdog submodule
	PomdogRef string // Git ref (branch/tag/commit) for pomdog submodule
}

var options struct {
	Verbose        bool
	NonInteractive bool
	SkipGit        bool
}

func main() {
	var config projectConfig
	flag.StringVar(&config.Dir, "o", ".", "parent directory for the new project")
	flag.StringVar(&config.Name, "name", "", "project name (lowercase alphanumeric and underscores)")
	flag.StringVar(&config.URL, "url", "", "project bundle identifier (e.g. com.example.my-game)")
	flag.StringVar(&config.PomdogDir, "pomdog", "", "path to the pomdog repository (for template files)")
	flag.StringVar(&config.PomdogURL, "pomdog-url", "", "git URL for pomdog submodule (default: auto-detect from pomdog dir)")
	flag.StringVar(&config.PomdogRef, "pomdog-ref", "", "git ref for pomdog submodule (branch, tag, or commit)")
	flag.BoolVar(&options.NonInteractive, "non-interactive", false, "non-interactive mode for CI and automation")
	flag.BoolVar(&options.SkipGit, "skip-git", false, "skip git init and submodule setup")
	flag.BoolVar(&options.Verbose, "verbose", false, "enable verbose output")
	flag.Parse()

	if !options.NonInteractive {
		config.PomdogDir = ask(
			"Where is the Pomdog directory? (e.g. path/to/pomdog)",
			config.PomdogDir,
			isValidPomdogDir)
		config.Dir = ask(
			"Where do you want to create the project?",
			defaultIfEmpty(config.Dir, "."),
			isValidPath)
		config.Name = ask(
			"Project name? (lowercase letters, digits, underscores; e.g. my_game)",
			config.Name,
			isValidName)
		if config.URL == "" {
			config.URL = "com.example." + strings.ReplaceAll(config.Name, "_", "-")
		}
		config.URL = ask(
			"Bundle identifier?",
			config.URL,
			isValidURL)
	}

	if config.PomdogDir == "" {
		fmt.Fprintln(os.Stderr, "error: pomdog directory is required (use -pomdog flag)")
		os.Exit(1)
	}
	if !isValidPomdogDir(config.PomdogDir) {
		fmt.Fprintln(os.Stderr, "error: invalid pomdog directory:", config.PomdogDir)
		os.Exit(1)
	}
	if config.Name == "" {
		fmt.Fprintln(os.Stderr, "error: project name is required (use -name flag)")
		os.Exit(1)
	}
	if !isValidName(config.Name) {
		fmt.Fprintln(os.Stderr, "error: invalid project name:", config.Name)
		fmt.Fprintln(os.Stderr, "  must start with a lowercase letter, followed by lowercase letters, digits, or underscores")
		os.Exit(1)
	}
	if config.URL == "" {
		config.URL = "com.example." + strings.ReplaceAll(config.Name, "_", "-")
	}
	if !isValidURL(config.URL) {
		fmt.Fprintln(os.Stderr, "error: invalid bundle identifier:", config.URL)
		os.Exit(1)
	}

	if config.PomdogURL == "" && !options.SkipGit {
		config.PomdogURL = detectPomdogURL(config.PomdogDir)
	}

	if err := createProject(config); err != nil {
		log.Fatal(err)
	}

	rootDir := filepath.Join(config.Dir, config.Name)
	fmt.Println(color.GreenString("Done."))
	fmt.Printf("\nTo get started:\n")
	fmt.Printf("  cd %s\n", rootDir)
	fmt.Printf("  ./tools/script/bootstrap.sh\n")
	fmt.Printf("  ./tools/script/assetbuild.sh\n\n")
}

func createProject(config projectConfig) error {
	rootDir := filepath.Join(config.Dir, config.Name)

	if _, err := os.Stat(rootDir); err == nil {
		return fmt.Errorf("directory already exists: %s", rootDir)
	}

	if err := os.MkdirAll(rootDir, os.ModePerm); err != nil {
		return fmt.Errorf("failed to create directory %s: %w", rootDir, err)
	}

	// 1. Initialize git repository first (before submodule add)
	if !options.SkipGit {
		fmt.Println("Initializing git repository...")
		if err := runGit(rootDir, "init"); err != nil {
			return fmt.Errorf("failed to initialize git: %w", err)
		}
	}

	// 2. Add pomdog as git submodule
	if !options.SkipGit {
		fmt.Println("Adding pomdog as git submodule...")
		if err := runGit(rootDir, "submodule", "add", config.PomdogURL, "pomdog"); err != nil {
			return fmt.Errorf("failed to add pomdog submodule: %w", err)
		}
		if config.PomdogRef != "" {
			if err := runGit(filepath.Join(rootDir, "pomdog"), "checkout", config.PomdogRef); err != nil {
				return fmt.Errorf("failed to checkout pomdog ref %s: %w", config.PomdogRef, err)
			}
		}

		// Initialize nested submodules (e.g. pomdog/thirdparty/ninja)
		fmt.Println("Initializing submodules...")
		if err := runGit(rootDir, "submodule", "update", "--init", "--recursive"); err != nil {
			return fmt.Errorf("failed to initialize submodules: %w", err)
		}
	}

	// 3. Copy template files from examples/quickstart
	fmt.Println("Copying template files...")
	if err := copyTemplateFiles(config.PomdogDir, rootDir); err != nil {
		return fmt.Errorf("failed to copy template files: %w", err)
	}

	// 4. Create tools directory structure and scripts
	fmt.Println("Creating build scripts...")
	if err := createToolsDir(rootDir, config.Name); err != nil {
		return fmt.Errorf("failed to create tools directory: %w", err)
	}

	// 5. Create .gitignore
	if err := writeFile(filepath.Join(rootDir, ".gitignore"), generateGitignore()); err != nil {
		return fmt.Errorf("failed to create .gitignore: %w", err)
	}

	// 6. Create build directory
	if err := os.MkdirAll(filepath.Join(rootDir, "build"), os.ModePerm); err != nil {
		return fmt.Errorf("failed to create build directory: %w", err)
	}

	// 7. Rename identifiers in all project files
	fmt.Println("Configuring project files...")
	if err := renameProjectFiles(rootDir, config.Name, config.URL); err != nil {
		return fmt.Errorf("failed to configure project files: %w", err)
	}

	return nil
}

func copyTemplateFiles(pomdogDir, rootDir string) error {
	templateDir := filepath.Join(pomdogDir, "examples", "quickstart")

	entries := []string{
		"assets",
		"platform/cocoa",
		"platform/emscripten",
		"platform/linux",
		"platform/win32",
		"source",
		"CMakeLists.txt",
		"README.md",
	}

	for _, entry := range entries {
		src := filepath.Join(templateDir, entry)
		dst := filepath.Join(rootDir, entry)
		if err := copyFiles(src, dst, nil); err != nil {
			return fmt.Errorf("failed to copy %s: %w", entry, err)
		}
		if options.Verbose {
			fmt.Printf("  copy: %s -> %s\n", src, dst)
		}
	}
	return nil
}

func createToolsDir(rootDir, name string) error {
	dirs := []string{
		"tools/cmd",
		"tools/pkg",
		"tools/script",
	}
	for _, dir := range dirs {
		if err := os.MkdirAll(filepath.Join(rootDir, dir), os.ModePerm); err != nil {
			return err
		}
	}

	type scriptEntry struct {
		path    string
		content string
	}

	scripts := []scriptEntry{
		{"tools/script/bootstrap.sh", generateBootstrapScript()},
		{"tools/script/build_tools.sh", generateBuildToolsScript()},
		{"tools/script/assetbuild.sh", generateAssetBuildScript(name)},
		{"tools/script/package_windows.sh", generatePackageWindowsScript(name)},
		{"tools/script/package_linux.sh", generatePackageLinuxScript(name)},
		{"tools/script/package_emscripten.sh", generatePackageEmscriptenScript(name)},
		{"tools/script/clang_format.sh", generateClangFormatScript()},
		{"tools/script/docs_format.sh", generateDocsFormatScript()},
		{"tools/bootstrap.toml", generateBootstrapToml()},
	}

	for _, s := range scripts {
		filePath := filepath.Join(rootDir, s.path)
		if err := os.WriteFile(filePath, []byte(s.content), 0755); err != nil {
			return fmt.Errorf("failed to create %s: %w", s.path, err)
		}
		if options.Verbose {
			fmt.Printf("  create: %s\n", s.path)
		}
	}

	return nil
}

func renameProjectFiles(rootDir, name, url string) error {
	// Replace bundle identifier in info.plist
	if err := replaceInFile(filepath.Join(rootDir, "platform/cocoa/info.plist"), func(s string) string {
		return strings.ReplaceAll(s, "com.example.quickstart", url)
	}); err != nil {
		return err
	}

	// Replace POMDOG_DIR and build paths in CMakeLists.txt
	if err := replaceInFile(filepath.Join(rootDir, "CMakeLists.txt"), func(s string) string {
		s = strings.Replace(s, `set(POMDOG_DIR "../..")`, `set(POMDOG_DIR "pomdog")`, 1)
		s = strings.Replace(s,
			`set(BUILD_CONTENT_DIR "${POMDOG_DIR}/build/quickstart/content")`,
			`set(BUILD_CONTENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${PRODUCT_NAME}/content")`, 1)
		s = strings.Replace(s,
			`set(BUILD_WORK_DIR "${POMDOG_DIR}/build/quickstart/work")`,
			`set(BUILD_WORK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${PRODUCT_NAME}/work")`, 1)
		s = strings.Replace(s,
			`set(SHIPPING_DIR "${POMDOG_DIR}/build/quickstart/shipping")`,
			`set(SHIPPING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/${PRODUCT_NAME}/shipping")`, 1)
		return s
	}); err != nil {
		return err
	}

	// Fix relative paths in README.md (template uses ../../docs/ which is relative
	// to examples/quickstart inside pomdog; the generated project has pomdog as a submodule)
	if err := replaceInFile(filepath.Join(rootDir, "README.md"), func(s string) string {
		s = strings.ReplaceAll(s, "../../docs/", "pomdog/docs/")
		s = strings.ReplaceAll(s, "../../tools/", "pomdog/tools/")
		return s
	}); err != nil {
		return err
	}

	// Replace quickstart identifier in all relevant files
	filesToRename := []string{
		"CMakeLists.txt",
		"README.md",
		"source/game_main.cpp",
		"source/game_main.h",
		"source/game_setup.cpp",
		"source/game_setup.h",
		"platform/cocoa/AppDelegate.mm",
		"platform/cocoa/main.mm",
		"platform/cocoa/info.plist",
		"platform/cocoa/Base.lproj/MainMenu.xib",
		"platform/linux/main.cpp",
		"platform/win32/main.cpp",
		"platform/emscripten/main.cpp",
		"platform/emscripten/index.html",
	}

	for _, file := range filesToRename {
		fp := filepath.Join(rootDir, file)
		if _, err := os.Stat(fp); os.IsNotExist(err) {
			continue
		}
		if err := replaceInFile(fp, func(s string) string {
			s = strings.ReplaceAll(s, "Quickstart", capitalizeFirst(name))
			s = strings.ReplaceAll(s, "QUICKSTART", strings.ToUpper(name))
			s = strings.ReplaceAll(s, "quickstart", name)
			return s
		}); err != nil {
			return fmt.Errorf("failed to replace identifiers in %s: %w", file, err)
		}
	}

	return nil
}

// MARK: Git helpers

func runGit(dir string, args ...string) error {
	cmd := exec.Command("git", args...)
	cmd.Dir = dir
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if options.Verbose {
		fmt.Printf("  git %s (in %s)\n", strings.Join(args, " "), dir)
	}
	return cmd.Run()
}

func detectPomdogURL(pomdogDir string) string {
	absDir, err := filepath.Abs(pomdogDir)
	if err != nil {
		return absDir
	}

	cmd := exec.Command("git", "remote", "get-url", "origin")
	cmd.Dir = absDir
	out, err := cmd.Output()
	if err != nil {
		// Fall back to absolute path
		return absDir
	}

	url := strings.TrimSpace(string(out))
	if url != "" {
		return url
	}
	return absDir
}

// MARK: File helpers

func replaceInFile(file string, replacer func(string) string) error {
	dat, err := os.ReadFile(file)
	if err != nil {
		return fmt.Errorf("failed to read %s: %w", file, err)
	}

	content := replacer(string(dat))

	f, err := os.Create(file)
	if err != nil {
		return fmt.Errorf("failed to create %s: %w", file, err)
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	defer w.Flush()

	if _, err := w.WriteString(content); err != nil {
		return fmt.Errorf("failed to write %s: %w", file, err)
	}

	return nil
}

func writeFile(path, content string) error {
	dir := filepath.Dir(path)
	if err := os.MkdirAll(dir, os.ModePerm); err != nil {
		return err
	}
	return os.WriteFile(path, []byte(content), 0644)
}

func copyFiles(src, dst string, ignoreDirs []string) error {
	stat, err := os.Stat(src)
	if err != nil {
		return err
	}
	if stat.IsDir() {
		dir, err := os.Open(src)
		if err != nil {
			return err
		}
		defer dir.Close()

		files, err := dir.Readdir(-1)
		if err != nil {
			return err
		}
		dir.Close()

		if err := os.MkdirAll(dst, os.ModePerm); err != nil {
			return err
		}

		ignoreDir := func(filename string) bool {
			for _, ignore := range ignoreDirs {
				if filename == ignore {
					return true
				}
			}
			return false
		}

		sem := semaphore.NewWeighted(4)
		var eg errgroup.Group

		for _, f := range files {
			file := f
			sem.Acquire(context.Background(), 1)
			eg.Go(func() error {
				defer sem.Release(1)
				if ignoreDir(file.Name()) {
					return nil
				}
				if file.IsDir() || file.Mode().IsRegular() {
					s := filepath.Join(src, file.Name())
					d := filepath.Join(dst, file.Name())
					if err := copyFiles(s, d, ignoreDirs); err != nil {
						return err
					}
				}
				return nil
			})
		}
		if err := eg.Wait(); err != nil {
			return fmt.Errorf("failed to copy files: %w", err)
		}
		return nil
	}
	return CopyFile(src, dst)
}

// MARK: Validation

func isValidPomdogDir(dir string) bool {
	stat, err := os.Stat(dir)
	if err != nil {
		return false
	}
	if !stat.IsDir() {
		return false
	}
	// Check for pomdog.h as a sanity check
	_, err = os.Stat(filepath.Join(dir, "pomdog", "pomdog.h"))
	return err == nil
}

func isValidPath(str string) bool {
	if str == "" {
		return false
	}
	re := regexp.MustCompile(`^[0-9a-zA-Z_\-.\\/+]+$`)
	return re.MatchString(str)
}

func isValidName(str string) bool {
	if str == "" {
		return false
	}
	re := regexp.MustCompile(`^[a-z][a-z0-9_]*$`)
	return re.MatchString(str)
}

func isValidURL(str string) bool {
	if str == "" {
		return false
	}
	re := regexp.MustCompile(`^[0-9a-zA-Z\-.]+$`)
	return re.MatchString(str)
}

// MARK: Interactive mode helpers

func ask(question, defaultAnswer string, validator func(string) bool) string {
	for {
		if defaultAnswer != "" {
			fmt.Print(color.GreenString("> %s [%s] ", question, defaultAnswer))
		} else {
			fmt.Print(color.GreenString("> %s ", question))
		}

		reader := bufio.NewReader(os.Stdin)
		answer, _ := reader.ReadString('\n')
		answer = strings.TrimSpace(answer)

		if answer == "" {
			if defaultAnswer != "" {
				return defaultAnswer
			}
			fmt.Println(color.RedString("Please enter your answer"))
			continue
		}

		if !validator(answer) {
			fmt.Println(color.RedString("Invalid input, please try again"))
			continue
		}
		return answer
	}
}

func defaultIfEmpty(s, fallback string) string {
	if s == "" {
		return fallback
	}
	return s
}

func capitalizeFirst(s string) string {
	if len(s) == 0 {
		return s
	}
	return strings.ToUpper(s[:1]) + s[1:]
}
