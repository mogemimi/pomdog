// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func main() {
	var options struct {
		outDir  string
		exclude string
		dryRun  bool
		verbose bool
	}

	flag.StringVar(&options.outDir, "o", "", "output directory for bundled shared libraries (required)")
	flag.StringVar(&options.exclude, "exclude", defaultExcludePatterns(), "comma-separated prefixes of libraries to exclude (e.g. linux-vdso,libm,libc,libpthread,libdl,librt,libgcc_s,ld-linux)")
	flag.BoolVar(&options.dryRun, "dry-run", false, "print libraries that would be copied without actually copying")
	flag.BoolVar(&options.verbose, "v", false, "verbose output")
	flag.Parse()

	if len(flag.Args()) == 0 {
		fmt.Fprintln(os.Stderr, "error: no input binary specified")
		fmt.Fprintln(os.Stderr, "usage: bundle-deps [options] <binary>")
		os.Exit(1)
	}
	if options.outDir == "" && !options.dryRun {
		fmt.Fprintln(os.Stderr, "error: -o <output directory> is required (or use -dry-run)")
		os.Exit(1)
	}

	binary := flag.Args()[0]

	excludePrefixes := parseExcludes(options.exclude)

	libs, err := collectDeps(binary)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: failed to run ldd on %q: %v\n", binary, err)
		os.Exit(1)
	}

	targets := filterLibs(libs, excludePrefixes)

	if len(targets) == 0 {
		fmt.Println("No non-system shared libraries to bundle.")
		return
	}

	if options.dryRun || options.verbose {
		fmt.Println("Shared libraries to bundle:")
		for _, lib := range targets {
			fmt.Printf("  %s -> %s\n", lib.soname, lib.path)
		}
	}
	if options.dryRun {
		return
	}

	if err := os.MkdirAll(options.outDir, 0o755); err != nil {
		fmt.Fprintf(os.Stderr, "error: failed to create output directory: %v\n", err)
		os.Exit(1)
	}

	for _, lib := range targets {
		dst := filepath.Join(options.outDir, lib.soname)
		if err := copyFile(lib.path, dst); err != nil {
			fmt.Fprintf(os.Stderr, "error: failed to copy %s: %v\n", lib.soname, err)
			os.Exit(1)
		}
		fmt.Printf("copied: %s -> %s\n", lib.path, dst)
	}

	fmt.Printf("\nBundled %d shared libraries into %s\n", len(targets), options.outDir)
}

type sharedLib struct {
	soname string
	path   string
}

// collectDeps runs `ldd` on the binary and parses the output.
func collectDeps(binary string) ([]sharedLib, error) {
	cmd := exec.Command("ldd", binary)
	out, err := cmd.Output()
	if err != nil {
		return nil, err
	}

	var libs []sharedLib
	scanner := bufio.NewScanner(strings.NewReader(string(out)))
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		// NOTE: Typical ldd output lines:
		//     ```
		//     libfoo.so.1 => /usr/lib/x86_64-linux-gnu/libfoo.so.1 (0x...)
		//     /lib64/ld-linux-x86-64.so.2 (0x...)
		//     linux-vdso.so.1 (0x...)
		//     ````
		if idx := strings.Index(line, " => "); idx >= 0 {
			soname := line[:idx]
			rest := line[idx+4:]
			// NOTE: rest: "/path/to/lib.so (0x...)" or "not found"
			if strings.Contains(rest, "not found") {
				libs = append(libs, sharedLib{soname: soname, path: ""})
				continue
			}
			if parenIdx := strings.Index(rest, " ("); parenIdx >= 0 {
				path := rest[:parenIdx]
				if path != "" {
					libs = append(libs, sharedLib{soname: soname, path: path})
				}
			}
		}
	}
	return libs, scanner.Err()
}

// filterLibs removes libraries whose soname starts with an excluded prefix.
func filterLibs(libs []sharedLib, excludePrefixes []string) []sharedLib {
	var result []sharedLib
	for _, lib := range libs {
		if lib.path == "" {
			continue
		}
		excluded := false
		for _, prefix := range excludePrefixes {
			if strings.HasPrefix(lib.soname, prefix) {
				excluded = true
				break
			}
		}
		if !excluded {
			result = append(result, lib)
		}
	}
	return result
}

func defaultExcludePatterns() string {
	// NOTE: Standard system libraries that should not be bundled.
	return strings.Join([]string{
		"linux-vdso",
		"ld-linux",
		"libc.so",
		"libm.so",
		"libpthread.so",
		"libdl.so",
		"librt.so",
		"libgcc_s.so",
		"libstdc++.so",
	}, ",")
}

func parseExcludes(s string) []string {
	parts := strings.Split(s, ",")
	var result []string
	for _, p := range parts {
		p = strings.TrimSpace(p)
		if p != "" {
			result = append(result, p)
		}
	}
	return result
}

func copyFile(src, dst string) error {
	srcInfo, err := os.Stat(src)
	if err != nil {
		return err
	}
	if !srcInfo.Mode().IsRegular() {
		return fmt.Errorf("non-regular source file: %s", src)
	}

	// NOTE: Resolve symlinks to copy the actual file content.
	realSrc, err := filepath.EvalSymlinks(src)
	if err != nil {
		return err
	}

	in, err := os.Open(realSrc)
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
	return out.Sync()
}
