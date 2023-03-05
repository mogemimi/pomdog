// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"time"
)

func compileHLSL(src, dst string, compileOptions compileOptions) ([]byte, error) {
	// Generate temporary object file path
	t := time.Now()
	fxo := dst + "-" + t.Format("20060102150405") + ".fxo"

	cmd := exec.Command(
		options.HLSLCompiler,
		"/nologo",
		"/T", compileOptions.ShaderProfile,
		"/E", compileOptions.EntryPoint,
		"/Fo", fxo,
		src)
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		return nil, fmt.Errorf("failed to compile \"%s\": %w", src, err)
	}

	if _, err := os.Stat(fxo); err != nil {
		return nil, fmt.Errorf("cannot find a .fxo file \"%s\": %w", src, err)
	}

	dat, err := ioutil.ReadFile(fxo)
	if err != nil {
		return nil, fmt.Errorf("cannot read a .fxo file \"%s\": %w", src, err)
	}
	if err := os.Remove(fxo); err != nil {
		return nil, fmt.Errorf("failed to remove a .fxo file \"%s\": %w", src, err)
	}
	return dat, nil
}
