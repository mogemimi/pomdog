// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

var options struct {
	Verbose      bool
	HLSLCompiler string
	Minify       bool
}

type compileOptions struct {
	EntryPoint    string
	ShaderProfile string
}

func main() {
	var compileOptions compileOptions
	flag.BoolVar(&options.Verbose, "verbose", false, "verbose")
	flag.BoolVar(&options.Minify, "minify", false, "minify")
	flag.StringVar(&compileOptions.EntryPoint, "entrypoint", "", "shader entry point")
	flag.StringVar(&compileOptions.ShaderProfile, "profile", "", "shader profile (e.g. vs_4_0, ps_4_0)")
	flag.StringVar(&options.HLSLCompiler, "fxc", "fxc", "path to fxc.exe")
	flag.Parse()

	for _, f := range flag.Args() {
		if err := createEmbeddedFile(f, compileOptions); err != nil {
			fmt.Fprintln(os.Stderr, err)
			os.Exit(1)
		}
	}
}

func createEmbeddedFile(src string, compileOptions compileOptions) error {
	ext := filepath.Ext(src)
	base := strings.TrimSuffix(filepath.Base(src), ext)
	dir := filepath.Dir(src)
	lang := findShaderLanguageFromExtension(ext)

	dst := filepath.Join(dir+".Embedded", base+".inc.h")

	content := ""

	switch lang {
	case shaderLanguageGLSL:
		prefix := "Builtin_GLSL_"
		dat, err := ioutil.ReadFile(src)
		if err != nil {
			return fmt.Errorf("failed to read a file \"%s\": %w", src, err)
		}
		content = createEmbeddedCode(base, prefix, minifyCode(string(dat)))
	case shaderLanguageMetal:
		if exec.Command("xcrun", "--help").Run() == nil {
			// NOTE: When running on Mac, check compilation result
			if _, err := compileMetal(src, dst); err != nil {
				return fmt.Errorf("failed to compile a shader \"%s\": %w", src, err)
			}
		}
		prefix := "Builtin_Metal_"
		dat, err := ioutil.ReadFile(src)
		if err != nil {
			return fmt.Errorf("failed to read a file \"%s\": %w", src, err)
		}
		content = createEmbeddedCode(base, prefix, minifyCode(string(dat)))
	case shaderLanguageHLSL:
		prefix := "BuiltinHLSL_"
		if options.Minify {
			dat, err := ioutil.ReadFile(src)
			if err != nil {
				return fmt.Errorf("failed to read a file \"%s\": %w", src, err)
			}
			content = createEmbeddedCode(base, prefix, minifyCode(string(dat)))
		} else {
			data, err := compileHLSL(src, dst, compileOptions)
			if err != nil {
				return fmt.Errorf("failed to compile a shader \"%s\": %w", src, err)
			}
			content = createEmbeddedBinary(base, prefix, binaryToByteArrayString(data))
		}
	}

	f, err := os.Create(dst)
	if err != nil {
		return fmt.Errorf("failed to create a file \"%s\": %w", dst, err)
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	defer w.Flush()

	if _, err := w.WriteString(content); err != nil {
		return fmt.Errorf("failed to write a file \"%s\": %w", dst, err)
	}

	return nil
}
