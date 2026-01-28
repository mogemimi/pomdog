// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"os"
)

func main() {
	var options struct {
		AppDir         string
		BuildDir       string
		PomdogDir      string
		CMakeGenerator string
		SkipCppBuild   bool
		SkipDownload   bool
	}
	flag.StringVar(&options.AppDir, "appdir", "./examples/app", "specify app directory")
	flag.StringVar(&options.BuildDir, "builddir", "./build", "specify build directory")
	flag.StringVar(&options.PomdogDir, "pomdogdir", ".", "specify pomdog directory")
	flag.StringVar(&options.CMakeGenerator, "cmake-generator", "", "specify CMake generator (e.g., 'Visual Studio 17', 'Visual Studio 18', 'Ninja')")
	flag.BoolVar(&options.SkipCppBuild, "skip-cpp", false, "skip cpp build")
	flag.BoolVar(&options.SkipDownload, "skip-download", false, "skip download")
	flag.Parse()

	config := &Config{}
	if err := config.ReadFiles(flag.Args()); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	if err := config.ExpandEnv(options.AppDir, options.BuildDir, options.PomdogDir); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	runArgs := &RunArgs{
		BuildDir:       options.BuildDir,
		PomdogDir:      options.PomdogDir,
		CMakeGenerator: options.CMakeGenerator,
		SkipCppBuild:   options.SkipCppBuild,
		SkipDownload:   options.SkipDownload,
	}

	if err := run(config, runArgs); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
