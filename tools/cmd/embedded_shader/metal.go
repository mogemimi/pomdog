package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"time"
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
		return nil, fmt.Errorf("failed to compile \"%s\": %w", src, err)
	}

	if _, err := os.Stat(obj); err != nil {
		return nil, fmt.Errorf("cannot find a .air file \"%s\": %w", src, err)
	}

	dat, err := ioutil.ReadFile(obj)
	if err != nil {
		return nil, fmt.Errorf("cannot read a .air file \"%s\": %w", src, err)
	}
	if err := os.Remove(obj); err != nil {
		return nil, fmt.Errorf("failed to remove a .air file \"%s\": %w", src, err)
	}
	return dat, nil
}
