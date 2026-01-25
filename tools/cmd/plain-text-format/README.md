# plain-text-format

A simple command-line tool for applying basic text formatting to files.

## Features

- **Replace CRLF with LF** (`-replace-crlf`): Convert Windows-style line endings (CRLF) to Unix-style (LF)
- **Remove trailing whitespace** (`-remove-space-eol`): Strip spaces and tabs from the end of each line
- **Normalize EOF newline** (`-remove-newline-eof`): Remove extra blank lines at the end of file and ensure exactly one trailing newline
- **Remove UTF-8 BOM** (`-remove-bom`): Remove unnecessary UTF-8 Byte Order Mark from the beginning of files
- **Replace tabs with spaces** (`-replace-tabs`): Convert hard tabs to 4 spaces

## Installation

```sh
cd tools/cmd/plain-text-format
go build
```

## Usage

```sh
plain-text-format [options] <file patterns...>
```

### Options

| Option | Description |
|--------|-------------|
| `-jobs N` | Run N jobs in parallel (default: number of CPUs on the system) |
| `-replace-crlf` | Replace CRLF line endings with LF |
| `-remove-space-eol` | Remove trailing spaces and tabs from each line |
| `-remove-newline-eof` | Ensure file ends with exactly one newline |
| `-remove-bom` | Remove UTF-8 BOM from the beginning of files |
| `-replace-tabs` | Replace hard tabs with 4 spaces |

### Examples

Apply all formatting options to C++ source files:

```sh
plain-text-format -replace-crlf -remove-space-eol -remove-newline-eof "src/*.cpp" "src/*.h"
```

Convert line endings only:

```sh
plain-text-format -replace-crlf "docs/*.md"
```

Remove trailing whitespace from all Go files:

```sh
plain-text-format -remove-space-eol "**/*.go"
```

## Notes

- Files are processed in parallel (default: number of CPUs, configurable via `-jobs`)
- Files are only written if changes are detected
- Supports glob patterns for file matching
