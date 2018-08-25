// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

package main

import (
	"io/ioutil"
	"os"
	"os/exec"
	"time"

	"github.com/pkg/errors"
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
		return nil, errors.Wrapf(err, "failed to compile \"%s\"", src)
	}

	if _, err := os.Stat(fxo); err != nil {
		return nil, errors.Wrapf(err, "cannot find a .fxo file \"%s\"", src)
	}

	dat, err := ioutil.ReadFile(fxo)
	if err != nil {
		return nil, errors.Wrapf(err, "cannot read a .fxo file \"%s\"", src)
	}
	if err := os.Remove(fxo); err != nil {
		return nil, errors.Wrapf(err, "failed to remove a .fxo file \"%s\"", src)
	}
	return dat, nil
}
