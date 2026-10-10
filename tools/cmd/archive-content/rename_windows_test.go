// Copyright mogemimi. Distributed under the MIT license.

//go:build windows

package main

import (
	"os"
	"path/filepath"
	"syscall"
	"testing"
	"unsafe"
)

// mapFile maps the file at path for reading with shareMode, as the memory-mapped
// archive volume of the engine does. It returns the mapped bytes and a function
// that unmaps them.
func mapFile(t *testing.T, path string, shareMode uint32) ([]byte, func()) {
	t.Helper()
	name, err := syscall.UTF16PtrFromString(path)
	if err != nil {
		t.Fatalf("syscall.UTF16PtrFromString() failed: %v", err)
	}
	file, err := syscall.CreateFile(name, syscall.GENERIC_READ, shareMode, nil, syscall.OPEN_EXISTING, syscall.FILE_ATTRIBUTE_NORMAL, 0)
	if err != nil {
		t.Fatalf("syscall.CreateFile() failed: %v", err)
	}
	mapping, err := syscall.CreateFileMapping(file, nil, syscall.PAGE_READONLY, 0, 0, nil)
	if err != nil {
		syscall.CloseHandle(file)
		t.Fatalf("syscall.CreateFileMapping() failed: %v", err)
	}
	address, err := syscall.MapViewOfFile(mapping, syscall.FILE_MAP_READ, 0, 0, 0)
	if err != nil {
		syscall.CloseHandle(mapping)
		syscall.CloseHandle(file)
		t.Fatalf("syscall.MapViewOfFile() failed: %v", err)
	}
	info, err := os.Stat(path)
	if err != nil {
		t.Fatalf("os.Stat() failed: %v", err)
	}
	// NOTE: MapViewOfFile() returns the address of memory outside the Go heap as uintptr.
	pointer := *(*unsafe.Pointer)(unsafe.Pointer(&address))
	view := unsafe.Slice((*byte)(pointer), info.Size())
	return view, func() {
		syscall.UnmapViewOfFile(address)
		syscall.CloseHandle(mapping)
		syscall.CloseHandle(file)
	}
}

func TestRenameFileOverMappedFile(t *testing.T) {
	dir := t.TempDir()
	path := writeTestFile(t, dir, "content.pak", []byte("old content"))

	t.Run("replaces a file that is mapped with delete sharing", func(t *testing.T) {
		view, unmap := mapFile(t, path, syscall.FILE_SHARE_READ|syscall.FILE_SHARE_DELETE)
		defer unmap()

		if err := writeFileByRename(path, []byte("new content")); err != nil {
			t.Fatalf("writeFileByRename() failed: %v", err)
		}
		if got := string(view); got != "old content" {
			t.Errorf("the mapped view holds %q, want the old content", got)
		}
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatalf("os.ReadFile() failed: %v", err)
		}
		if got := string(data); got != "new content" {
			t.Errorf("the file holds %q, want the new content", got)
		}
	})

	t.Run("fails to replace a file that is mapped without delete sharing", func(t *testing.T) {
		_, unmap := mapFile(t, path, syscall.FILE_SHARE_READ)
		defer unmap()

		if err := writeFileByRename(path, []byte("newer content")); err == nil {
			t.Error("writeFileByRename() replaced a file that does not share delete access")
		}
	})

	entries, err := os.ReadDir(dir)
	if err != nil {
		t.Fatalf("os.ReadDir() failed: %v", err)
	}
	for _, entry := range entries {
		if filepath.Ext(entry.Name()) == ".tmp" {
			t.Errorf("the temporary file %s remains", entry.Name())
		}
	}
}
