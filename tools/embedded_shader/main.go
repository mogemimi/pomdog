// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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

	"github.com/pkg/errors"
)

var options struct {
	Verbose      bool
	HLSLCompiler string
}

type compileOptions struct {
	EntryPoint    string
	ShaderProfile string
}

func main() {
	var compileOptions compileOptions
	flag.BoolVar(&options.Verbose, "verbose", false, "verbose")
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

	dst := filepath.Join(dir+".Embedded", base+".inc.hpp")

	content := ""

	switch lang {
	case shaderLanguageGLSL:
		prefix := "Builtin_GLSL_"
		dat, err := ioutil.ReadFile(src)
		if err != nil {
			return errors.Wrapf(err, "failed to read a file \"%s\"", src)
		}
		content = createEmbeddedCode(base, prefix, minifyCode(string(dat)))
	case shaderLanguageMetal:
		if exec.Command("xcrun", "--help").Run() == nil {
			// NOTE: When running on Mac, check compilation result
			if _, err := compileMetal(src, dst); err != nil {
				return errors.Wrapf(err, "failed to compile a shader \"%s\"", src)
			}
		}
		prefix := "Builtin_Metal_"
		dat, err := ioutil.ReadFile(src)
		if err != nil {
			return errors.Wrapf(err, "failed to read a file \"%s\"", src)
		}
		content = createEmbeddedCode(base, prefix, minifyCode(string(dat)))
	case shaderLanguageHLSL:
		prefix := "BuiltinHLSL_"
		data, err := compileHLSL(src, dst, compileOptions)
		if err != nil {
			return errors.Wrapf(err, "failed to compile a shader \"%s\"", src)
		}
		content = createEmbeddedBinary(base, prefix, binaryToByteArrayString(data))
	}

	f, err := os.Create(dst)
	if err != nil {
		return errors.Wrapf(err, "failed to create a file \"%s\"", dst)
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	defer w.Flush()

	if _, err := w.WriteString(content); err != nil {
		return errors.Wrapf(err, "failed to write a file \"%s\"", dst)
	}

	return nil
}
