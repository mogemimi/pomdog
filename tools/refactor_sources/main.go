// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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

	"github.com/pkg/errors"
)

var options struct {
	inplaceEdit bool
}

func main() {
	flag.BoolVar(&options.inplaceEdit, "i", false, "Inplace edit files, if specified")
	flag.Parse()

	files, err := getFiles(flag.Args())
	if err != nil {
		log.Fatalln(err)
	}

	for _, src := range files {
		err := refactor(src, options.inplaceEdit)
		if err != nil {
			log.Fatalln(err)
		}
	}
}

func getFiles(args []string) (files []string, err error) {
	for _, src := range args {
		stat, err := os.Stat(src)
		if err != nil {
			return nil, errors.Wrapf(err, "invalid argument \"%s\"", src)
		}
		if stat.IsDir() {
			err := filepath.Walk(src, func(path string, info os.FileInfo, err error) error {
				if err != nil {
					return errors.Wrapf(err, "failed to walk directory \"%s\"", path)
				}
				if !info.IsDir() && info.Mode().IsRegular() && isSourceFile(path) {
					files = append(files, path)
				}
				return nil
			})
			if err != nil {
				return files, errors.Wrapf(err, "failed to get file list \"%s\"", src)
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
		".hpp",
		".mm",
		".m",
		".h",
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
		return errors.Wrapf(err, "failed to read a file \"%s\"", file)
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
