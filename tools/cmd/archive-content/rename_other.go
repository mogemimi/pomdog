// Copyright mogemimi. Distributed under the MIT license.

//go:build !windows

package main

import "os"

// renameFile renames oldPath to newPath, replacing the file at newPath. A
// process that has the old file at newPath open keeps reading the old content.
func renameFile(oldPath, newPath string) error {
	return os.Rename(oldPath, newPath)
}
