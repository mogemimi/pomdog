package main

import (
	"io/ioutil"
	"os"
	"os/exec"
	"time"

	"github.com/pkg/errors"
)

func compileMetal(src, dst string) ([]byte, error) {
	// Generate temporary object file path
	t := time.Now()
	obj := dst + "-" + t.Format("20060102150405") + ".air"

	cmd := exec.Command(
		"xcrun",
		"-sdk", "macosx",
		"metal",
		"-c", src,
		"-o", obj)
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		return nil, errors.Wrapf(err, "failed to compile \"%s\"", src)
	}

	if _, err := os.Stat(obj); err != nil {
		return nil, errors.Wrapf(err, "cannot find a .air file \"%s\"", src)
	}

	dat, err := ioutil.ReadFile(obj)
	if err != nil {
		return nil, errors.Wrapf(err, "cannot read a .air file \"%s\"", src)
	}
	if err := os.Remove(obj); err != nil {
		return nil, errors.Wrapf(err, "failed to remove a .air file \"%s\"", src)
	}
	return dat, nil
}
