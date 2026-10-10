// Copyright mogemimi. Distributed under the MIT license.

//go:build windows

package main

import (
	"errors"
	"os"
	"path/filepath"
	"syscall"
	"unsafe"
)

var procSetFileInformationByHandle = syscall.NewLazyDLL("kernel32.dll").NewProc("SetFileInformationByHandle")

const (
	fileRenameInfoEx              = 22
	fileRenameFlagReplaceIfExists = 0x1
	fileRenameFlagPOSIXSemantics  = 0x2
	accessDelete                  = 0x00010000

	errorInvalidFunction  syscall.Errno = 1
	errorNotSupported     syscall.Errno = 50
	errorInvalidParameter syscall.Errno = 87
)

// fileRenameInfo is FILE_RENAME_INFO. The new file name continues past the end
// of FileName.
type fileRenameInfo struct {
	Flags          uint32
	RootDirectory  syscall.Handle
	FileNameLength uint32
	FileName       [1]uint16
}

// renameFile renames oldPath to newPath, replacing the file at newPath. A
// process that has the old file at newPath open or memory-mapped keeps reading
// the old content, as long as it shares delete access.
//
// os.Rename() cannot replace a memory-mapped file on Windows, so renameFile
// renames with POSIX semantics, and falls back to os.Rename() on file systems
// and Windows versions without them.
func renameFile(oldPath, newPath string) error {
	err := renameFilePOSIX(oldPath, newPath)
	if errors.Is(err, errorInvalidFunction) || errors.Is(err, errorNotSupported) || errors.Is(err, errorInvalidParameter) {
		return os.Rename(oldPath, newPath)
	}
	if err != nil {
		return &os.LinkError{Op: "rename", Old: oldPath, New: newPath, Err: err}
	}
	return nil
}

// renameFilePOSIX renames oldPath to newPath with SetFileInformationByHandle()
// and FILE_RENAME_FLAG_POSIX_SEMANTICS.
func renameFilePOSIX(oldPath, newPath string) error {
	absNewPath, err := filepath.Abs(newPath)
	if err != nil {
		return err
	}
	oldName, err := syscall.UTF16PtrFromString(oldPath)
	if err != nil {
		return err
	}
	newName, err := syscall.UTF16FromString(absNewPath)
	if err != nil {
		return err
	}

	handle, err := syscall.CreateFile(
		oldName,
		accessDelete|syscall.SYNCHRONIZE,
		syscall.FILE_SHARE_READ|syscall.FILE_SHARE_WRITE|syscall.FILE_SHARE_DELETE,
		nil,
		syscall.OPEN_EXISTING,
		syscall.FILE_ATTRIBUTE_NORMAL,
		0)
	if err != nil {
		return err
	}
	defer syscall.CloseHandle(handle)

	// NOTE: newName ends with a NUL character, which FileNameLength excludes.
	nameLength := (len(newName) - 1) * 2
	size := unsafe.Sizeof(fileRenameInfo{}) + uintptr(nameLength)

	// NOTE: Allocate 64-bit words, so that the buffer is aligned for fileRenameInfo.
	buffer := make([]uint64, (size+7)/8)
	info := (*fileRenameInfo)(unsafe.Pointer(&buffer[0]))
	info.Flags = fileRenameFlagReplaceIfExists | fileRenameFlagPOSIXSemantics
	info.FileNameLength = uint32(nameLength)
	copy(unsafe.Slice(&info.FileName[0], len(newName)), newName)

	result, _, callErr := procSetFileInformationByHandle.Call(
		uintptr(handle),
		fileRenameInfoEx,
		uintptr(unsafe.Pointer(info)),
		size)
	if result == 0 {
		return callErr
	}
	return nil
}
