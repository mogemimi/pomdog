# clang-format-all

A command-line tool for running clang-format on multiple files in parallel.

## Features

- **Parallel execution**: Process multiple files concurrently (default: number of CPUs on the system) for faster formatting
- **Auto-discovery on Windows**: Automatically searches for clang-format in common installation paths:
  - System PATH
  - LLVM installation (`C:\Program Files\LLVM\bin`)
  - Visual Studio 2019/2022/2026 (Professional and Community editions)
- **Custom executable path**: Option to specify a custom clang-format executable
- **Glob pattern support**: Format multiple files using glob patterns

## Installation

```sh
cd tools/cmd/clang-format-all
go build
```

## Usage

```sh
clang-format-all [options] <file patterns...>
```

### Options

| Option | Description |
|--------|-------------|
| `-jobs N` | Run N jobs in parallel (default: number of CPUs on the system) |
| `-clangformat <path>` | Path to clang-format executable (optional, auto-detected if not specified) |

### Examples

Format all C++ source files in the src directory:

```sh
clang-format-all "src/*.cpp" "src/*.h"
```

Format files with a specific clang-format executable:

```sh
clang-format-all -clangformat "/path/to/clang-format" "src/*.cpp"
```

Format header files recursively (using shell expansion):

```sh
clang-format-all include/**/*.h
```

## Notes

- Files are processed in parallel (default: number of CPUs, configurable via `-jobs`) for improved performance
- On Windows, clang-format is automatically discovered from common installation locations if not in PATH
- Uses clang-format's `-i` flag to format files in-place
- Requires a `.clang-format` configuration file in your project for consistent formatting
