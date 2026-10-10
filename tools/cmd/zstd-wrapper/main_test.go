// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"
)

// NOTE: When this variable is set, the test binary stands for zstd, so the
// tests need no zstd executable.
const fakeZstdEnv = "ZSTD_WRAPPER_FAKE_ZSTD"

// oldTime is the modification time that the fake zstd gives its output.
var oldTime = time.Date(2000, 1, 1, 0, 0, 0, 0, time.UTC)

func TestMain(m *testing.M) {
	if mode := os.Getenv(fakeZstdEnv); mode != "" {
		os.Exit(fakeZstd(mode, os.Args[1:]))
	}
	os.Exit(m.Run())
}

// fakeZstd writes its arguments to the file after -o and gives the file an old
// modification time, as zstd copies the time of its input. With the "fail"
// mode, it fails without writing the file.
func fakeZstd(mode string, args []string) int {
	output, err := outputFile(args)
	if err != nil {
		return 2
	}
	if mode == "fail" {
		fmt.Fprintln(os.Stderr, "fake zstd failed")
		return 7
	}
	if err := os.WriteFile(output, []byte(strings.Join(args, " ")), 0o644); err != nil {
		return 3
	}
	if err := os.Chtimes(output, oldTime, oldTime); err != nil {
		return 4
	}
	return 0
}

func TestRunTouchesOutput(t *testing.T) {
	t.Setenv(fakeZstdEnv, "write")
	output := filepath.Join(t.TempDir(), "a.json.zst")
	args := []string{"-q", "-f", "-19", "a.json", "-o", output}

	start := time.Now().Add(-time.Second)
	exitCode, err := run(os.Args[0], args)
	if err != nil || exitCode != 0 {
		t.Fatalf("run() = %d, %v, want 0, nil", exitCode, err)
	}

	buf, err := os.ReadFile(output)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	if got, want := string(buf), strings.Join(args, " "); got != want {
		t.Errorf("zstd got arguments %q, want %q", got, want)
	}

	stat, err := os.Stat(output)
	if err != nil {
		t.Fatalf("os.Stat() failed: %v", err)
	}
	if stat.ModTime().Before(start) {
		t.Errorf("output modification time is %v, want the time when zstd ran", stat.ModTime())
	}
}

func TestRunFailure(t *testing.T) {
	t.Setenv(fakeZstdEnv, "fail")
	output := filepath.Join(t.TempDir(), "a.json.zst")

	exitCode, err := run(os.Args[0], []string{"-q", "a.json", "-o", output})
	if err != nil {
		t.Fatalf("run() failed: %v", err)
	}
	if exitCode != 7 {
		t.Errorf("exit code %d, want the exit code 7 of zstd", exitCode)
	}
	if _, err := os.Stat(output); !os.IsNotExist(err) {
		t.Errorf("os.Stat() = %v, want no output file", err)
	}
}

func TestOutputFile(t *testing.T) {
	for _, tc := range []struct {
		args []string
		want string
		err  string
	}{
		{[]string{"-q", "a.json", "-o", "a.json.zst"}, "a.json.zst", ""},
		{[]string{"--train", "-o", "text.dict", "--filelist=files"}, "text.dict", ""},
		{[]string{"-q", "a.json"}, "", "-o is required"},
		{[]string{"-q", "a.json", "-o"}, "", "-o has no file"},
		{[]string{"-o", "a.zst", "-o", "b.zst"}, "", "-o is given more than once"},
	} {
		got, err := outputFile(tc.args)
		if tc.err != "" {
			if err == nil || !strings.Contains(err.Error(), tc.err) {
				t.Errorf("outputFile(%q) = %q, %v, want an error with %q", tc.args, got, err, tc.err)
			}
			continue
		}
		if err != nil || got != tc.want {
			t.Errorf("outputFile(%q) = %q, %v, want %q", tc.args, got, err, tc.want)
		}
	}
}
