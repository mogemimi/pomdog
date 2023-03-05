// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func main() {
	var options struct {
		currentDir string
		prefix     string
		outFile    string
	}

	flag.StringVar(&options.currentDir, "C", "", "current directory")
	flag.StringVar(&options.prefix, "p", "", "prefix")
	flag.StringVar(&options.outFile, "o", "", "output file name")
	flag.Parse()

	if len(options.currentDir) == 0 {
		path, err := os.Getwd()
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
		options.currentDir = path
	}
	if !filepath.IsAbs(options.currentDir) {
		if p, err := filepath.Abs(options.currentDir); err == nil {
			options.currentDir = p
		}
	}
	if len(options.prefix) == 0 {
		options.prefix = filepath.Base(options.currentDir)
	}
	if len(options.outFile) == 0 {
		options.outFile = filepath.Join(options.currentDir, options.prefix+".zip")
	}

	intermediateDir, err := ioutil.TempDir("", options.prefix)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	defer os.RemoveAll(intermediateDir)

	{
		cmd := exec.Command(
			"git", "archive",
			"--prefix", options.prefix+"/",
			"-o", filepath.Join(intermediateDir, options.prefix+".zip"),
			"HEAD")
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		if err := cmd.Run(); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
	}

	submodules := []string{}
	hashes := []string{}

	{
		commands := []string{
			"echo", "$path/$sha1",
		}

		options := []string{
			"submodule", "--quiet", "foreach",
			strings.Join(commands, " "),
		}

		var stdout bytes.Buffer
		cmd := exec.Command("git", options...)
		cmd.Stdout = &stdout
		cmd.Stderr = os.Stderr
		if err := cmd.Run(); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}

		for _, s := range strings.Split(stdout.String(), "\n") {
			if strings.HasPrefix(s, "Entering '") {
				continue
			}
			if len(s) == 0 {
				continue
			}
			submodules = append(submodules, filepath.Dir(s))
			hashes = append(hashes, filepath.Base(s))
		}
	}

	for _, s := range submodules {
		dir := filepath.Join(intermediateDir, s)
		if err := os.MkdirAll(dir, os.ModePerm); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
	}

	{
		commands := []string{
			"git", "archive",
			"--prefix", options.prefix + "/$path/",
			"--output", filepath.Join(intermediateDir, "$path", "$sha1.zip"),
			"HEAD",
		}

		options := []string{
			"submodule", "--quiet", "foreach",
			strings.Join(commands, " "),
		}

		cmd := exec.Command("git", options...)
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		if err := cmd.Run(); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
	}

	for i, s := range submodules {
		hash := hashes[i]
		cmd := exec.Command("unzip", "-o", filepath.Join(s, hash+".zip"))
		cmd.Stderr = os.Stderr
		cmd.Dir = intermediateDir
		if err := cmd.Run(); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
	}

	{
		cmd := exec.Command("unzip", "-o", options.prefix+".zip")
		cmd.Stderr = os.Stderr
		cmd.Dir = intermediateDir
		if err := cmd.Run(); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
	}

	{
		depDir := options.prefix + "/dependencies/"
		cmd := exec.Command(
			"zip",
			options.outFile,
			"-r", options.prefix+"/",
			"-x", "*.DS_Store",
			"-x", "*/__MACOSX",
			"-x", depDir+"Catch2/.conan/*",
			"-x", depDir+"Catch2/.github/*",
			"-x", depDir+"Catch2/artwork/*",
			"-x", depDir+"Catch2/CMake/*",
			"-x", depDir+"Catch2/contrib/*",
			"-x", depDir+"Catch2/docs/*",
			"-x", depDir+"Catch2/examples/*",
			"-x", depDir+"Catch2/misc/*",
			"-x", depDir+"Catch2/projects/*",
			"-x", depDir+"Catch2/scripts/*",
			"-x", depDir+"Catch2/third_party/*",
			"-x", depDir+"Catch2/conanfile.py",
			"-x", depDir+"glew/auto/*",
			"-x", depDir+"glew/build/*",
			"-x", depDir+"glew/config/*",
			"-x", depDir+"glew/doc/*",
			"-x", depDir+"glew/*.sh",
			"-x", depDir+"glew/*.pc.in",
			"-x", depDir+"libpng/contrib/*",
			"-x", depDir+"libpng/arm/*",
			"-x", depDir+"libpng/intel/*",
			"-x", depDir+"libpng/mips/*",
			"-x", depDir+"libpng/powerpc/*",
			"-x", depDir+"libpng/projects/*",
			"-x", depDir+"libpng/scripts/*",
			"-x", depDir+"libpng/tests/*",
			"-x", depDir+"libpng/CHANGES",
			"-x", depDir+"libpng/compile",
			"-x", depDir+"libpng/configure",
			"-x", depDir+"libpng/depcomp",
			"-x", depDir+"libpng/install-sh",
			"-x", depDir+"libpng/missing",
			"-x", depDir+"libpng/test-driver",
			"-x", depDir+"libpng/TODO",
			"-x", depDir+"libpng/*.png",
			"-x", depDir+"mbedtls/.github/*",
			"-x", depDir+"mbedtls/ChangeLog.d/*",
			"-x", depDir+"mbedtls/docs/*",
			"-x", depDir+"mbedtls/doxygen/*",
			"-x", depDir+"mbedtls/programs/*",
			"-x", depDir+"mbedtls/scripts/*",
			"-x", depDir+"mbedtls/tests/*",
			"-x", depDir+"mbedtls/visualc/*",
			"-x", depDir+"mbedtls/ChangeLog",
			"-x", depDir+"nanosvg/example/*",
			"-x", depDir+"rapidjson/bin/*",
			"-x", depDir+"rapidjson/CMakeModules/*",
			"-x", depDir+"rapidjson/contrib/*",
			"-x", depDir+"rapidjson/doc/*",
			"-x", depDir+"rapidjson/docker/*",
			"-x", depDir+"rapidjson/example/*",
			"-x", depDir+"rapidjson/test/*",
			"-x", depDir+"rapidjson/thirdparty/*",
			"-x", depDir+"rapidjson/CHANGELOG.md",
			"-x", depDir+"stb/.github/*",
			"-x", depDir+"stb/data/*",
			"-x", depDir+"stb/deprecated/*",
			"-x", depDir+"stb/docs/*",
			"-x", depDir+"stb/tests/*",
			"-x", depDir+"stb/tools/*",
			"-x", depDir+"utfcpp/.circleci/*",
			"-x", depDir+"utfcpp/extern/*",
			"-x", depDir+"utfcpp/samples/*",
			"-x", depDir+"utfcpp/tests/*",
			"-x", depDir+"zlib/amiga/*",
			"-x", depDir+"zlib/contrib/*",
			"-x", depDir+"zlib/doc/*",
			"-x", depDir+"zlib/examples/*",
			"-x", depDir+"zlib/msdos/*",
			"-x", depDir+"zlib/nintendods/*",
			"-x", depDir+"zlib/old/*",
			"-x", depDir+"zlib/os400/*",
			"-x", depDir+"zlib/qnx/*",
			"-x", depDir+"zlib/test/*",
			"-x", depDir+"zlib/watcom/*",
			"-x", depDir+"zlib/win32/*",
			"-x", depDir+"zlib/ChangeLog",
			"-x", depDir+"zlib/configure",
			"-x", depDir+"zlib/zlib2ansi",
			"-x", depDir+"*/.travis.yml",
			"-x", depDir+"*/.travis.yml.conan",
			"-x", depDir+"*/appveyor.yml",
			"-x", depDir+"*.autopkg",
			"-x", depDir+"*.cmake",
			"-x", depDir+"*.cmake.in",
			"-x", depDir+"*.cmakein",
			"-x", depDir+"*/CMakeLists.txt",
			"-x", depDir+"*/Makefile",
			"-x", depDir+"*/Makefile.in",
			"-x", depDir+"*/premake4.lua",
		)
		cmd.Stderr = os.Stderr
		cmd.Dir = intermediateDir
		if err := cmd.Run(); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
	}
}
