# bootstrap-toolchain

A bootstrapping tool for setting up the Pomdog game engine development environment. This is the first tool to run after cloning the repository.

## Features

- **Build C++ tools**: Compile essential C++ tools from source (ninja, spirv-cross, glslang, flatc)
- **Download external tools**: Automatically download pre-built binaries (ffmpeg, Python embeddable)
- **Build Go tools**: Compile Go-based development tools (clang-format-all, plain-text-format, etc.)
- **Generate code**: Generate C++ and Go code from FlatBuffers schemas
- **Cross-platform**: Supports Windows (x64/ARM64), macOS (Intel/Apple Silicon), and Linux (x64/ARM64)

## Prerequisites

- **Go**: Required to build bootstrap-toolchain itself and other Go tools
- **CMake**: Required for building C++ tools
- **C++ compiler**: Visual Studio (Windows), Xcode/Clang (macOS), or Clang/GCC (Linux)
- **Ninja** (Linux only): Required for building C++ tools on Linux

## Installation

```sh
cd tools/cmd/bootstrap-toolchain
go build -o ../../../build/tools/bootstrap-toolchain
```

## Usage

```sh
bootstrap-toolchain [options] <config files...>
```

### Options

| Option | Description |
|--------|-------------|
| `-appdir <path>` | Specify app directory (default: `./examples/app`) |
| `-builddir <path>` | Specify build directory (default: `./build`) |
| `-pomdogdir <path>` | Specify Pomdog repository root directory (default: `.`) |
| `-cmake-generator <name>` | Specify CMake generator (e.g., `Visual Studio 17`, `Visual Studio 18`, `Ninja`, `Xcode`) |
| `-skip-cpp` | Skip building C++ tools (ninja, spirv-cross, glslang, flatc) |
| `-skip-download` | Skip downloading external tools |

### Examples

Full bootstrap (first time setup):

```sh
./build/tools/bootstrap-toolchain \
    --pomdogdir . \
    --builddir ./build \
    --appdir . \
    ./tools/cmd/bootstrap-toolchain/bootstrap.toml
```

Rebuild Go tools only (skip C++ builds and downloads):

```sh
./build/tools/bootstrap-toolchain \
    --pomdogdir . \
    --builddir ./build \
    --appdir . \
    --skip-cpp \
    --skip-download \
    ./tools/cmd/bootstrap-toolchain/bootstrap.toml
```

Or use the provided scripts:

```sh
# Full bootstrap
./tools/script/bootstrap.sh

# Rebuild Go tools only
./tools/script/build_tools.sh
```

## Configuration

The tool reads TOML configuration files that specify:

- **go_tools**: Go tools to build
- **download_tools**: External tools to download (platform-specific)
- **schemas**: FlatBuffers schemas for code generation

Example configuration (`bootstrap.toml`):

```toml
[[go_tools]]
go_dir = "${POMDOG_DIR}/tools/cmd"
tools = [
    "clang-format-all",
    "plain-text-format",
]

[[download_tools]]
url = "https://example.com/tool-win64.zip"
platform = "windows_amd64"
in_file = "tool/bin/tool.exe"
out_file = "tool.exe"
sha256 = "f05e28d161c6b15af64a7cb7f08b4a22b3a6b03eee71baee24ea557b3bdd5798"

[[download_tools]]
url = "https://example.com/tool-mac-arm64.zip"
platform = "mac_arm64"
in_file = "tool"
out_file = "tool"
md5 = "d41d8cd98f00b204e9800998ecf8427e"
```

### Supported Platforms

The `platform` field in `[[download_tools]]` supports the following values:

| Platform | Description |
|----------|-------------|
| `windows_amd64` | Windows x64 (Intel/AMD 64-bit) |
| `windows_arm64` | Windows ARM64 |
| `mac_amd64` | macOS Intel (x86_64) |
| `mac_arm64` | macOS Apple Silicon (M1/M2/M3/M4) |
| `linux_amd64` | Linux x64 (Intel/AMD 64-bit) |
| `linux_arm64` | Linux ARM64 |

### Checksum Verification

Downloaded files can be verified using checksums. Both `sha256` and `md5` fields are supported, and you can specify either or both:

```toml
[[download_tools]]
url = "https://example.com/tool.zip"
platform = "windows_amd64"
in_file = "tool.exe"
out_file = "tool.exe"
sha256 = "abc123..."  # SHA-256 checksum (recommended)
md5 = "def456..."     # MD5 checksum (optional, can be used together with sha256)
```

## Output

After running, the following tools will be available in `build/tools/`:

- `ninja` - Build system
- `flatc` - FlatBuffers compiler
- `glslangValidator` - GLSL/SPIR-V compiler
- `spirv-cross` - SPIR-V shader converter
- `ffmpeg` - Media processing tool
- `clang-format-all` - Parallel clang-format runner
- `plain-text-format` - Text file formatter
- And other Go tools defined in the configuration

## Notes

- Run this tool from the repository root directory
- Downloaded files are cached in `build/downloads/`
- Go tools are built in parallel for faster setup
