package pkg

import (
	"fmt"
	"io"
	"os"

	"github.com/pkg/errors"
)

func CopyFile(src, dst string) error {
	srcInfo, err := os.Stat(src)
	if err != nil {
		return err
	}
	if !srcInfo.Mode().IsRegular() {
		return fmt.Errorf("non-regular source file %s (%q)", srcInfo.Name(), srcInfo.Mode().String())
	}
	dstInfo, err := os.Stat(dst)
	if err != nil {
		if !os.IsNotExist(err) {
			return errors.Wrapf(err, "failed to copy to \"%s\"", dst)
		}
	} else {
		if !dstInfo.Mode().IsRegular() {
			return fmt.Errorf("non-regular destination file %s (%q)", dstInfo.Name(), dstInfo.Mode().String())
		}
		if os.SameFile(srcInfo, dstInfo) {
			return nil
		}
	}

	in, err := os.Open(src)
	if err != nil {
		return err
	}
	defer in.Close()
	out, err := os.Create(dst)
	if err != nil {
		return err
	}
	defer func() {
		cerr := out.Close()
		if err == nil {
			err = cerr
		}
	}()
	if _, err = io.Copy(out, in); err != nil {
		return err
	}
	err = out.Sync()
	return err
}
