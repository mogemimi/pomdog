// Copyright mogemimi. Distributed under the MIT license.

package depfile

import (
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"strings"
)

type DepFile struct {
	OutFile  string
	DepFiles []string
}

// Generate generates a GCC-style depfile as a string.
func (dep *DepFile) Generate() string {
	escape := func(s string) string {
		s = filepath.ToSlash(s)
		s = strings.ReplaceAll(s, ":", "\\:")
		return s
	}

	str := fmt.Sprintf("%s:", escape(dep.OutFile))
	for _, file := range dep.DepFiles {
		str += fmt.Sprintf(" %s", escape(file))
	}
	str += "\n"
	return str
}

// WriteFile writes the GCC-style depfile to a file.
func (dep *DepFile) WriteFile(outFile string, perm fs.FileMode) error {
	content := dep.Generate()
	dat := []byte(content)

	if err := os.WriteFile(outFile, dat, perm); err != nil {
		return fmt.Errorf("failed to write depfile: %s: %w", outFile, err)
	}
	return nil
}
