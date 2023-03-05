// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"context"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/fatih/color"
	pkg "github.com/mogemimi/pomdog/tools/pkg"
	"golang.org/x/sync/errgroup"
	"golang.org/x/sync/semaphore"
)

type projectConfig struct {
	Dir       string
	Name      string
	URL       string
	PomdogDir string
}

var options struct {
	Verbose        bool
	NonInteractive bool
}

func main() {
	var config projectConfig
	flag.StringVar(&config.Dir, "o", ".", "project directory path")
	flag.StringVar(&config.Name, "name", "", "project name")
	flag.StringVar(&config.URL, "url", "com.example.quickstart", "project url")
	flag.StringVar(&config.PomdogDir, "pomdog", "", "pomdog directory path")
	flag.BoolVar(&options.NonInteractive, "non-interactive", false, "non-interactive mode for CI")
	flag.BoolVar(&options.Verbose, "verbose", false, "verbose")
	flag.Parse()

	if !options.NonInteractive {
		config.PomdogDir = ask("Where is a Pomdog directory? (e.g. path/to/pomdog)", "", isValidPomdogDir)
		config.Dir = ask("Where do you want to create your new gamedev project?", config.Dir, isValidPath)
		config.Name = ask("What is your project name? (e.g. mygame)", "", isValidName)
		config.URL = strings.Replace(config.URL, "quickstart", config.Name, 1)
		config.URL = strings.Replace(config.URL, "_", "-", -1)
		config.URL = ask("What is your project URL?", config.URL, isValidURL)
	}

	if len(config.PomdogDir) == 0 {
		fmt.Fprintln(os.Stderr, "please specify a pomdog directory (e.g. Use -pomdog path/to/pomdog option)")
		os.Exit(1)
	}
	if !isValidPomdogDir(config.PomdogDir) {
		fmt.Fprintln(os.Stderr, "invalid pomdog directory")
		os.Exit(1)
	}
	if len(config.Name) == 0 {
		fmt.Fprintln(os.Stderr, "please specify a project name (e.g. Use -name MyGame option)")
		os.Exit(1)
	}
	if !isValidName(config.Name) {
		fmt.Fprintln(os.Stderr, "invalid project name")
		os.Exit(1)
	}
	if !isValidPath(config.Dir) {
		fmt.Fprintln(os.Stderr, "invalid output directory")
		os.Exit(1)
	}
	if !isValidURL(config.URL) {
		fmt.Fprintln(os.Stderr, "invalid project url")
		os.Exit(1)
	}

	if err := createProject(config); err != nil {
		log.Fatal(err)
	}

	fmt.Println(color.GreenString("Done."))
}

func createProject(config projectConfig) error {
	rootDir := filepath.Join(config.Dir, config.Name)

	if _, err := os.Stat(rootDir); err == nil {
		return fmt.Errorf("%s directory already exists", rootDir)
	}
	if err := os.MkdirAll(rootDir, os.ModePerm); err != nil {
		return fmt.Errorf("failed to make a directory \"%s\": %w", rootDir, err)
	}
	if err := copyTemplateFiles(config.PomdogDir, rootDir); err != nil {
		return fmt.Errorf("failed to copy template files: %w", err)
	}
	if err := copyFrameworkFiles(config.PomdogDir, rootDir); err != nil {
		return fmt.Errorf("failed to copy framework files: %w", err)
	}
	if err := copyThirdPartyFiles(config.PomdogDir, rootDir); err != nil {
		return fmt.Errorf("failed to copy third-party files: %w", err)
	}

	if err := renameContentByURL(rootDir, config.URL, "platform/cocoa/info.plist"); err != nil {
		return fmt.Errorf("failed to rename URL: %w", err)
	}

	sourceFiles := []string{
		"README.md",
		"CMakeLists.txt",
		"source/game_main.cpp",
		"source/game_main.h",
		"platform/cocoa/AppDelegate.mm",
		"platform/cocoa/main.mm",
		"platform/cocoa/info.plist",
		"platform/cocoa/Base.lproj/MainMenu.xib",
		"platform/linux/main.cpp",
		"platform/win32/main.cpp",
	}
	for _, f := range sourceFiles {
		if err := renameContentByIdent(rootDir, config.Name, f); err != nil {
			return fmt.Errorf("failed to rename content: %w", err)
		}
	}
	if err := renameCMakefile(rootDir, config.Name, "CMakeLists.txt"); err != nil {
		return fmt.Errorf("failed to replace cmake path: %w", err)
	}
	if err := renamePomdogCMakefile(rootDir, config.Name, "third_party/pomdog/cmake/pomdog/CMakeLists.txt"); err != nil {
		return fmt.Errorf("failed to replace cmake path: %w", err)
	}

	thirdPartyCMakeFiles := []string{
		"third_party/pomdog/cmake/dependencies/giflib/CMakeLists.txt",
		"third_party/pomdog/cmake/dependencies/glew/CMakeLists.txt",
		"third_party/pomdog/cmake/dependencies/libpng/CMakeLists.txt",
		"third_party/pomdog/cmake/dependencies/mbedtls/CMakeLists.txt",
		"third_party/pomdog/cmake/dependencies/stb/CMakeLists.txt",
		"third_party/pomdog/cmake/dependencies/zlib/CMakeLists.txt",
	}
	for _, f := range thirdPartyCMakeFiles {
		if err := renameThirdPartyCMakefile(rootDir, config.Name, f); err != nil {
			return fmt.Errorf("failed to rename content: %w", err)
		}
	}
	return nil
}

func copyTemplateFiles(sourceRoot, destRoot string) error {
	files := []string{
		"content",
		"platform/cocoa",
		"platform/linux",
		"platform/win32",
		"source",
		".gitignore",
		"CMakeLists.txt",
		"README.md",
	}
	ignoreDirs := []string{}
	for _, f := range files {
		src := filepath.Join(sourceRoot, "examples/quickstart", f)
		dst := filepath.Join(destRoot, f)
		if err := copyFiles(src, dst, ignoreDirs); err != nil {
			return fmt.Errorf("failed to copy from \"%s\" to \"%s\": %w", src, dst, err)
		}
		if options.Verbose {
			fmt.Println(src, "=>", dst)
		}
	}
	return nil
}

func copyFrameworkFiles(sourceRoot, destRoot string) error {
	files := []string{
		"cmake/dependencies",
		"cmake/pomdog",
		"pomdog",
		"LICENSE.md",
		"README.md",
		".gitignore",
	}
	ignoreDirs := []string{
		".git",
		".github",
	}
	for _, f := range files {
		src := filepath.Join(sourceRoot, f)
		dst := filepath.Join(filepath.Join(destRoot, "third_party/pomdog"), f)
		if err := copyFiles(src, dst, ignoreDirs); err != nil {
			return fmt.Errorf("failed to copy from \"%s\" to \"%s\": %w", src, dst, err)
		}
		if options.Verbose {
			fmt.Println(src, "=>", dst)
		}
	}
	return nil
}

func copyThirdPartyFiles(sourceRoot, destRoot string) error {
	files := []string{
		"giflib",
		"libpng",
		"mbedtls/3rdparty/everest",
		"mbedtls/include",
		"mbedtls/library",
		"nanosvg/src",
		"rapidjson/include",
		"stb",
		"utfcpp/source",
		"vendor",
		"zlib",
	}
	ignoreDirs := []string{
		".git",
		".github",
		"auto",
		"bin",
		"build",
		"ChangeLog.d",
		"contrib",
		"gtest",
		"data",
		"deprecated",
		"doc",
		"docs",
		"doxygen",
		"example",
		"extern",
		"pic",
		"samples",
		"scripts",
		"tests",
		"tools",
	}

	for _, f := range files {
		src := filepath.Join(sourceRoot, "dependencies", f)
		dst := filepath.Join(filepath.Join(destRoot, "third_party"), f)
		if err := copyFiles(src, dst, ignoreDirs); err != nil {
			return fmt.Errorf("failed to copy from \"%s\" to \"%s\": %w", src, dst, err)
		}
		if options.Verbose {
			fmt.Println(src, "=>", dst)
		}
	}

	return nil
}

func renameContentByURL(rootDir, url, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		return strings.Replace(content, "com.example.quickstart", url, -1)
	})
	return fmt.Errorf("failed to rename URL in \"%s\": %w", file, err)
}

func renameContentByIdent(rootDir, ident, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		content = strings.Replace(content, "quickstart", ident, -1)
		content = strings.Replace(content, "QUICKSTART", strings.ToUpper(ident), -1)
		return content
	})
	return fmt.Errorf("failed to rename identifier in \"%s\": %w", file, err)
}

func renameCMakefile(rootDir, ident, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		content = strings.Replace(
			content,
			`set(POMDOG_DIR "../..")`,
			`set(POMDOG_DIR "third_party/pomdog")`,
			1)

		return content
	})
	return fmt.Errorf("failed to rename identifier in \"%s\": %w", file, err)
}

func renamePomdogCMakefile(rootDir, ident, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		content = strings.Replace(
			content,
			`${POMDOG_DIR}/dependencies`,
			`${POMDOG_DIR}/..`,
			-1)
		return content
	})
	return fmt.Errorf("failed to rename identifier in \"%s\": %w", file, err)
}

func renameThirdPartyCMakefile(rootDir, ident, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		content = strings.Replace(
			content,
			`set(THIRDPARTY_DIR "../../../dependencies")`,
			`set(THIRDPARTY_DIR "../../../..")`,
			1)
		return content
	})
	return fmt.Errorf("failed to rename identifier in \"%s\": %w", file, err)
}

func replaceFileContent(file string, replacer func(string) string) error {
	dat, err := ioutil.ReadFile(file)
	if err != nil {
		return fmt.Errorf("failed to read a file \"%s\": %w", file, err)
	}

	content := string(dat)
	content = replacer(content)

	f, err := os.Create(file)
	if err != nil {
		return fmt.Errorf("failed to create a file \"%s\": %w", file, err)
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	defer w.Flush()

	if _, err := w.WriteString(content); err != nil {
		return fmt.Errorf("failed to write a file \"%s\": %w", file, err)
	}

	return nil
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

		err = os.MkdirAll(dst, os.ModePerm)
		if err != nil {
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
	return pkg.CopyFile(src, dst)
}

func isValidPomdogDir(dir string) bool {
	stat, err := os.Stat(dir)
	if err != nil {
		return false
	}
	return stat.IsDir()
}

func isValidPath(str string) bool {
	var re = regexp.MustCompile(`(?m)^[0-9a-zA-Z_\-\.\\/\+]+$`)
	return len(re.FindAllString(str, -1)) == 1
}

func isValidName(str string) bool {
	var re = regexp.MustCompile(`(?m)^[0-9a-zA-Z_]+$`)
	return len(re.FindAllString(str, -1)) == 1
}

func isValidURL(str string) bool {
	var re = regexp.MustCompile(`(?m)^[0-9a-zA-Z\-\.]+$`)
	return len(re.FindAllString(str, -1)) == 1
}

func ask(question, defaultAnswer string, validator func(string) bool) string {
	for {
		if len(defaultAnswer) > 0 {
			fmt.Print(color.GreenString("> %s [%s] ", question, defaultAnswer))
		} else {
			fmt.Print(color.GreenString("> %s ", question))
		}

		reader := bufio.NewReader(os.Stdin)
		answer, _ := reader.ReadString('\n')
		answer = strings.Trim(answer, " \t\n\r")

		if len(answer) == 0 {
			if len(defaultAnswer) > 0 {
				break
			}
			fmt.Println(color.RedString("Please enter your answer"))
			continue
		}

		if !validator(answer) {
			fmt.Println(color.RedString("You must enter something"))
			continue
		}
		return answer
	}
	return defaultAnswer
}
