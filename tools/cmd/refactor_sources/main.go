// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"
)

func main() {
	var options struct {
		all bool
	}

	flag.BoolVar(&options.all, "all", false, "all file")
	flag.Parse()

	files, err := getFiles(flag.Args())
	if err != nil {
		log.Fatalln(err)
	}

	if options.all {
		if s, err := os.Stat("./pomdog"); err != nil {
			log.Fatalln(err)
		} else if !s.IsDir() {
			log.Fatalln("pomdog is not directory")
		}

		dirs := []string{
			"CMakeLists.txt",
			"cmake/dependencies/*/CMakeLists.txt",
			"cmake/pomdog/CMakeLists.txt",
			"cmake/pomdog/*.cmake",
			"examples/*/*/*.h",
			"examples/*/*/*.cpp",
			"examples/*/*/*.mm",
			"examples/*/*/*/*.h",
			"examples/*/*/*/*.cpp",
			"examples/FeatureShowcase/Content/Shaders/*.glsl",
			"examples/FeatureShowcase/Content/Shaders/*.hlsl",
			"examples/FeatureShowcase/Content/Shaders/*.metal",
			"examples/QuickStart/Content/*.glsl",
			"examples/QuickStart/Content/*.hlsl",
			"examples/QuickStart/Content/*.metal",
			"examples/*/CMakeLists.txt",
			"pomdog/*.h",
			"pomdog/*/*.h",
			"pomdog/*/*/*.h",
			"pomdog/*/*/*/*.h",
			"pomdog/*/*.cpp",
			"pomdog/*/*/*.cpp",
			"pomdog/*/*/*/*.cpp",
			"pomdog/*/*.mm",
			"pomdog/*/*/*.mm",
			"pomdog/*/*/*/*.mm",
			"pomdog/*/*.glsl",
			"pomdog/*/*/*.glsl",
			"pomdog/*/*/*/*.glsl",
			"pomdog/*/*.hlsl",
			"pomdog/*/*/*.hlsl",
			"pomdog/*/*/*/*.hlsl",
			"pomdog/*/*.metal",
			"pomdog/*/*/*.metal",
			"pomdog/*/*/*/*.metal",
			"test/*.cpp",
			"test/*/*.h",
			"test/*/*.cpp",
			"test/CMakeLists.txt",
		}

		for _, dir := range dirs {
			f, err := filepath.Glob(dir)
			if err != nil {
				log.Fatalln(err)
			}
			files = append(files, f...)
		}
	}

	for _, src := range files {
		err := refactor(src, true)
		if err != nil {
			log.Fatalln(err)
		}
	}
}

func getFiles(args []string) (files []string, err error) {
	for _, src := range args {
		stat, err := os.Stat(src)
		if err != nil {
			return nil, fmt.Errorf("invalid argument \"%s\": %w", src, err)
		}
		if stat.IsDir() {
			err := filepath.Walk(src, func(path string, info os.FileInfo, err error) error {
				if err != nil {
					return fmt.Errorf("failed to walk directory \"%s\": %w", path, err)
				}
				if !info.IsDir() && info.Mode().IsRegular() && isSourceFile(path) {
					files = append(files, path)
				}
				return nil
			})
			if err != nil {
				return files, fmt.Errorf("failed to get file list \"%s\": %w", src, err)
			}
		} else if stat.Mode().IsRegular() {
			files = append(files, src)
		}
	}
	return files, nil
}

func isSourceFile(path string) bool {
	ext := filepath.Ext(path)
	matchers := []string{
		".cpp",
		".h",
		".mm",
		".m",
		".c",
		".glsl",
		".hlsl",
		".metal",
		".inc",
		".cmake",
	}
	for _, m := range matchers {
		if m == ext {
			return true
		}
	}
	return false
}

func removeBOM(content string) string {
	return strings.TrimLeft(content, "\xef\xbb\xbf")
}

func replaceCRLFWithLF(content string) string {
	return strings.Replace(content, "\r\n", "\n", -1)
}

func replaceHardTabsWithWhiteSpaces(content string) string {
	return strings.Replace(content, "\t", "    ", -1)
}

func removeUnnecessaryWhitespace(content string) string {
	lines := strings.Split(content, "\n")
	for i, line := range lines {
		lines[i] = strings.TrimRight(line, " \t")
	}
	return strings.Join(lines, "\n")
}

func removeWhitespaceFromEOF(content string) string {
	return strings.TrimRight(content, " \n\t") + "\n"
}

func refactor(file string, inplaceEdit bool) error {
	dat, err := ioutil.ReadFile(file)
	if err != nil {
		return fmt.Errorf("failed to read a file \"%s\": %w", file, err)
	}

	content := string(dat)
	content = removeBOM(content)
	content = replaceCRLFWithLF(content)
	content = replaceHardTabsWithWhiteSpaces(content)
	content = removeUnnecessaryWhitespace(content)
	content = removeWhitespaceFromEOF(content)

	if !inplaceEdit {
		fmt.Println(content)
		return nil
	}

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
