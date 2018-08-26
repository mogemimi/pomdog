// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/fatih/color"
	"github.com/pkg/errors"
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
	flag.StringVar(&config.URL, "url", "com.example.QuickStart", "project url")
	flag.StringVar(&config.PomdogDir, "pomdog", "", "pomdog directory path")
	flag.BoolVar(&options.NonInteractive, "non-interactive", false, "non-interactive mode for CI")
	flag.BoolVar(&options.Verbose, "verbose", false, "verbose")
	flag.Parse()

	if !options.NonInteractive {
		config.PomdogDir = ask("Where is a Pomdog directory? (e.g. path/to/pomdog)", "", isValidPomdogDir)
		config.Dir = ask("Where do you want to create your new gamedev project?", config.Dir, isValidPath)
		config.Name = ask("What is your project name? (e.g. MyGame)", "", isValidName)
		config.URL = strings.Replace(config.URL, "QuickStart", config.Name, 1)
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
		return errors.New(rootDir + " directory already exists")
	}
	if err := os.MkdirAll(rootDir, os.ModePerm); err != nil {
		return errors.Wrapf(err, "failed to make a directory \"%s\"", rootDir)
	}
	if err := copyTemplateFiles(config.PomdogDir, rootDir); err != nil {
		return errors.Wrapf(err, "failed to copy template files")
	}
	if err := copyFrameworkFiles(config.PomdogDir, rootDir); err != nil {
		return errors.Wrapf(err, "failed to copy framework files")
	}
	if err := copyThirdPartyFiles(config.PomdogDir, rootDir); err != nil {
		return errors.Wrapf(err, "failed to copy third-party files")
	}

	if err := renameContentByURL(rootDir, config.URL, "Platform.Cocoa/Info.plist"); err != nil {
		return errors.Wrapf(err, "failed to rename URL")
	}

	sourceFiles := []string{
		"README.md",
		"CMakeLists.txt",
		"Source/QuickStartGame.cpp",
		"Source/QuickStartGame.hpp",
		"Platform.Cocoa/AppDelegate.mm",
		"Platform.Cocoa/GameViewController.mm",
		"Platform.Cocoa/Info.plist",
		"Platform.Cocoa/Base.lproj/MainMenu.xib",
		"Platform.Win32/main.cpp",
		"Platform.X11/main.cpp",
	}
	for _, f := range sourceFiles {
		if err := renameContentByIdent(rootDir, config.Name, f); err != nil {
			return errors.Wrapf(err, "failed to rename content")
		}
	}
	if err := renameCMakefile(rootDir, config.Name, "CMakeLists.txt"); err != nil {
		return errors.Wrapf(err, "failed to replace cmake path")
	}
	if err := renamePomdogCMakefile(rootDir, config.Name, "ThirdParty/pomdog/build/pomdog/CMakeLists.txt"); err != nil {
		return errors.Wrapf(err, "failed to replace cmake path")
	}

	thirdPartyCMakeFiles := []string{
		"ThirdParty/pomdog/build/dependencies/giflib/CMakeLists.txt",
		"ThirdParty/pomdog/build/dependencies/glew/CMakeLists.txt",
		"ThirdParty/pomdog/build/dependencies/libpng/CMakeLists.txt",
		"ThirdParty/pomdog/build/dependencies/zlib/CMakeLists.txt",
	}
	for _, f := range thirdPartyCMakeFiles {
		if err := renameThirdPartyCMakefile(rootDir, config.Name, f); err != nil {
			return errors.Wrapf(err, "failed to rename content")
		}
	}

	renameTargetFiles := []string{
		"Source/QuickStartGame.cpp",
		"Source/QuickStartGame.hpp",
	}
	for _, f := range renameTargetFiles {
		src := filepath.Join(rootDir, f)
		dst := filepath.Join(rootDir, strings.Replace(f, "QuickStart", config.Name, 1))
		if err := os.Rename(src, dst); err != nil {
			return errors.Wrapf(err, "failed to rename file from \"%s\" to \"%s\"", src, dst)
		}
	}
	return nil
}

func copyTemplateFiles(sourceRoot, destRoot string) error {
	files := []string{
		"Content",
		"Platform.Cocoa",
		"Platform.Win32",
		"Platform.X11",
		"Source",
		".gitignore",
		"CMakeLists.txt",
		"README.md",
	}
	for _, f := range files {
		src := filepath.Join(sourceRoot, "examples/QuickStart", f)
		dst := filepath.Join(destRoot, f)
		if err := copyFiles(src, dst); err != nil {
			return errors.Wrapf(err, "failed to copy from \"%s\" to \"%s\"", src, dst)
		}
		if options.Verbose {
			fmt.Println(src, "=>", dst)
		}
	}
	return nil
}

func copyFrameworkFiles(sourceRoot, destRoot string) error {
	files := []string{
		"build/dependencies",
		"build/pomdog",
		"include",
		"src",
		"LICENSE.md",
		"README.md",
		".gitignore",
	}
	for _, f := range files {
		src := filepath.Join(sourceRoot, f)
		dst := filepath.Join(filepath.Join(destRoot, "ThirdParty/pomdog"), f)
		if err := copyFiles(src, dst); err != nil {
			return errors.Wrapf(err, "failed to copy from \"%s\" to \"%s\"", src, dst)
		}
		if options.Verbose {
			fmt.Println(src, "=>", dst)
		}
	}
	return nil
}

func copyThirdPartyFiles(sourceRoot, destRoot string) error {
	files := []string{
		"glew",
		"libpng",
		"rapidjson",
		"SDL_GameControllerDB",
		"stb",
		"utfcpp",
		"vendor",
		"zlib",
	}
	for _, f := range files {
		src := filepath.Join(sourceRoot, "dependencies", f)
		dst := filepath.Join(filepath.Join(destRoot, "ThirdParty"), f)
		if err := copyFiles(src, dst); err != nil {
			return errors.Wrapf(err, "failed to copy from \"%s\" to \"%s\"", src, dst)
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
		return strings.Replace(content, "com.example.QuickStart", url, -1)
	})
	return errors.Wrapf(err, "failed to rename URL in \"%s\"", file)
}

func renameContentByIdent(rootDir, ident, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		content = strings.Replace(content, "QuickStart", ident, -1)
		content = strings.Replace(content, "QUICKSTART", strings.ToUpper(ident), -1)
		return content
	})
	return errors.Wrapf(err, "failed to rename identifier in \"%s\"", file)
}

func renameCMakefile(rootDir, ident, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		content = strings.Replace(
			content,
			`set(POMDOG_DIR "../..")`,
			`set(POMDOG_DIR "ThirdParty/pomdog")`,
			-1)
		return content
	})
	return errors.Wrapf(err, "failed to rename identifier in \"%s\"", file)
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
	return errors.Wrapf(err, "failed to rename identifier in \"%s\"", file)
}

func renameThirdPartyCMakefile(rootDir, ident, source string) error {
	file := filepath.Join(rootDir, source)
	err := replaceFileContent(file, func(content string) string {
		content = strings.Replace(
			content,
			`set(THIRDPARTY_DIR "../../../dependencies")`,
			`set(THIRDPARTY_DIR "../../../..")`,
			-1)
		return content
	})
	return errors.Wrapf(err, "failed to rename identifier in \"%s\"", file)
}

func replaceFileContent(file string, replacer func(string) string) error {
	dat, err := ioutil.ReadFile(file)
	if err != nil {
		return errors.Wrapf(err, "failed to read a file \"%s\"", file)
	}

	content := string(dat)
	content = replacer(content)

	f, err := os.Create(file)
	if err != nil {
		return errors.Wrapf(err, "failed to create a file \"%s\"", file)
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	defer w.Flush()

	if _, err := w.WriteString(content); err != nil {
		return errors.Wrapf(err, "failed to write a file \"%s\"", file)
	}

	return nil
}

func copyFiles(src, dst string) error {
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

		err = os.MkdirAll(dst, os.ModePerm)
		if err != nil {
			return err
		}

		for _, f := range files {
			if f.IsDir() || f.Mode().IsRegular() {
				s := filepath.Join(src, f.Name())
				d := filepath.Join(dst, f.Name())
				if err := copyFiles(s, d); err != nil {
					return err
				}
			}
		}
		return nil
	}
	return copyFile(src, dst)
}

func copyFile(src, dst string) error {
	srcInfo, err := os.Stat(src)
	if err != nil {
		return err
	}
	if !srcInfo.Mode().IsRegular() {
		return fmt.Errorf("non-regular source file %s (%q)", srcInfo.Name(), srcInfo.Mode().String())
	}
	dstInfo, err := os.Stat(dst)
	if err != nil {
		if !os.IsNotExist(err) {
			return errors.Wrapf(err, "failed to copy to \"%s\"", dst)
		}
	} else {
		if !dstInfo.Mode().IsRegular() {
			return fmt.Errorf("non-regular destination file %s (%q)", dstInfo.Name(), dstInfo.Mode().String())
		}
		if os.SameFile(srcInfo, dstInfo) {
			return nil
		}
	}

	in, err := os.Open(src)
	if err != nil {
		return err
	}
	defer in.Close()
	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer func() {
		cerr := out.Close()
		if err == nil {
			err = cerr
		}
	}()
	if _, err = io.Copy(out, in); err != nil {
		return err
	}
	err = out.Sync()
	return err
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
