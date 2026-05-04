# Asset Pipeline and Runtime

This document describes how Pomdog processes assets at build time and loads them at runtime. It covers the build script, shader compilation, the archive format, the virtual file system (VFS), and how all the pieces fit together.

For shipping a finished game (creating a distributable package), see [Shipping](shipping.md).

## Overview

```
Source Assets         Build Pipeline                 Runtime
─────────────    ───────────────────────────    ─────────────────────
 .slang          shader-ninja-gen + ninja        VFS mount
 .ttf            copy-ninja-gen                  vfs::open()
 .png/.wav/...   archive-ninja-gen               file->read()
                 subninja-gen                         │
                        │                             ▼
                        ▼                        Loader functions
                  content.idx + content.pak
```

The pipeline is driven by `tools/script/assetbuild.sh`, which orchestrates a sequence of Go-based command-line tools. The final output is a pair of files — an **index** (`.idx`) and a **pack** (`.pak`) — that the engine mounts at runtime through the VFS layer.

## Running the Asset Build

Before building assets, the pipeline tools must be bootstrapped:

```sh
./tools/script/bootstrap.sh
```

This compiles the Go-based tools and downloads external binaries (e.g. `slangc`, `spirv-cross`, `ninja`) into `build/tools/`.

Then build all assets for every example application:

```sh
./tools/script/assetbuild.sh
```

Asset output is written to `build/<app>/` for each application listed in the script (`quickstart`, `pong`, `feature_showcase`).

> **Note:** `assetbuild.sh` is written for the built-in example applications. For your own application, use it as a reference and write a similar script that calls the same tools.

## How It Works

`assetbuild.sh` generates several independent Ninja build files, then combines them with the `subninja-gen` tool into a single top-level `build.ninja`. A single `ninja` invocation runs everything in parallel with full incremental build support — only changed assets are rebuilt.

The individual build stages are:

1. **Shader build (engine)** — `shader-ninja-gen` generates a Ninja file that compiles engine shaders from `assets/shaders/shaderbuild.toml`.
2. **Shader build (app)** — Same tool, for the application's own shaders in `examples/<app>/assets/shaders/shaderbuild.toml`.
3. **Asset copy (engine)** — `copy-ninja-gen` generates a Ninja file that copies engine assets (fonts, etc.) into the content directory.
4. **Asset copy (app)** — Same tool, for the application's own assets (textures, models, etc.).
5. **Archive** — `archive-ninja-gen` generates a Ninja file that packs everything into `content.idx` + `content.pak`, producing separate archives for each platform (windows, macos, linux, web).
6. **Combined build** — `subninja-gen` aggregates all the above Ninja files using Ninja's `subninja` feature.

Each stage can also be run independently by invoking `ninja` on the individual `.ninja` file.

### Cleaning

To clean built assets, either run `ninja -t clean` in the build directory, or simply delete the application's build folder (e.g. `build/feature_showcase`).

## Build Output Directory Structure

Each application's asset build produces the following directory layout under `build/<app>/`:

```
build/<app>/
├── shaderbuild/          # Shader compilation intermediates
│   ├── shaders_pomdog.ninja
│   └── shaders_app.ninja
├── copybuild/            # Asset copy build files
│   ├── copy_pomdog.ninja
│   └── copy_app.ninja
├── content/              # Converted assets (pre-archive)
│   ├── shaders/
│   │   ├── glsl/
│   │   ├── webgl/
│   │   ├── d3d11/
│   │   ├── metal/
│   │   ├── vk/
│   │   └── reflect/
│   ├── fonts/
│   ├── textures/
│   └── ...
├── archive/
│   └── build/            # Auto-generated archive recipes
├── archivebuild/
│   ├── build.ninja
│   ├── windows/
│   │   └── content.idx-debug
│   ├── macos/
│   │   └── content.idx-debug
│   ├── linux/
│   │   └── content.idx-debug
│   └── web/
│       └── content.idx-debug
├── shipping/
│   ├── windows/          # Windows platform archive
│   │   ├── content.idx
│   │   └── content.pak
│   ├── macos/            # macOS platform archive
│   │   ├── content.idx
│   │   └── content.pak
│   ├── linux/            # Linux platform archive
│   │   ├── content.idx
│   │   └── content.pak
│   └── web/              # WebGL / Emscripten archive
│       ├── content.idx
│       └── content.pak
└── build.ninja           # Top-level combined build file
```

Key points:

- **`content/`** contains converted assets before archiving. During development, this directory can be mounted as a VFS overlay so you can edit individual files without rebuilding the archive (see [Virtual File System](#virtual-file-system-vfs)).
- **`shipping/windows/`**, **`shipping/macos/`**, **`shipping/linux/`**, and **`shipping/web/`** contain the archived assets for each platform. Platform-specific packaging scripts further assemble these into distributable packages (see [Shipping](shipping.md)).
- **`archivebuild/`** contains debug index files (`content.idx-debug`) that map human-readable paths to their hash keys. These are not included in shipping output.

## Shader Compilation

All shaders are written in [Slang](https://github.com/shader-slang/slang) (`.slang` files) and compiled to SPIR-V, then cross-compiled to every target backend (GLSL, HLSL, Metal, SPIR-V) via `spirv-cross`. The `shader-ninja-gen` tool reads a `shaderbuild.toml` recipe and generates a Ninja build file that orchestrates the full pipeline.

For details on the shader compilation toolchain, SPIR-V post-processing, known pitfalls, and the GLSL → Slang conversion reference, see [Shader Compilation](shader-compilation.md).

## Archive System

### File Layout

The archive system produces two companion files:

| File | Description |
|------|-------------|
| `content.idx` | FlatBuffers index — sorted array of xxHash-64 keys mapping to file offsets |
| `content.pak` | Binary blob — concatenated raw file data |
| `content.idx-debug` | Debug-only — maps human-readable paths to their hash keys |

### FlatBuffers Schema (`schemas/archive.fbs`)

```flatbuffers
table ArchiveEntry {
    start_offset : uint32;       // Byte offset into the .pak file
    uncompressed_size : uint32;
    compressed_size : uint32;
    compressed : bool;           // Reserved for future lz4/zstd compression
}

table ArchiveKey {
    key : uint64(key);           // xxHash-64 of the virtual file path
    index : uint32;              // Index into the entries array
}

table Archive {
    keys : [ArchiveKey];         // Sorted for O(log n) binary search
    entries : [ArchiveEntry];
}
```

### Hashing

All path-to-key lookups use xxHash with a fixed seed:

```go
// tools/pkg/stringhash/string_hash.go
const Seed32 = uint32(20160723)   // xxHash-32 for shader reflection names
const Seed64 = uint64(20160723)   // xxHash-64 for archive file keys
```

### Archive Recipe

The `archive-content` tool accepts one or more TOML recipe files that list which files to include. Recipes can optionally target specific platforms:

```toml
# assets/archive/archive_fonts.toml
[[group]]
paths = [
    "fonts/NotoEmoji-Medium.ttf",
    "fonts/Roboto-Medium.ttf",
    "fonts/UbuntuMono-Bold.ttf",
]
```

```toml
# Generated by shader-archive-gen
[[group]]
target_platforms = ["windows", "linux", "macos"]
paths = [
    "shaders/glsl/basic_effect_vs.glsl",
    "shaders/glsl/basic_effect_ps.glsl",
]

[[group]]
target_platforms = ["emscripten"]
paths = [
    "shaders/webgl/basic_effect_vs.glsl",
    "shaders/webgl/basic_effect_ps.glsl",
]
```

When `--platform` is passed to `archive-content`, groups whose `target_platforms` do not include that platform are skipped, producing a platform-specific archive.

The `shader-archive-gen` tool can automatically generate archive recipes from `shaderbuild.toml`, listing all compiled shader outputs for each platform. This avoids manually maintaining archive recipes for large numbers of shader files.

## Virtual File System (VFS)

### Overview

The VFS layer (`pomdog/vfs/`) provides a unified file access API that abstracts over physical directories and packed archives. All runtime file loading goes through VFS, so the same game code works both in development mode (loose files on disk) and in shipping builds (archive packs).

### API

```cpp
#include "pomdog/vfs/file_system.h"

// Create a VFS context
auto [fs, err] = vfs::create();

// Mount a physical directory
vfs::mount(fs, "/assets", physicalPath, {.readOnly = true, .overlayFS = true});

// Mount a packed archive (.idx + .pak via memory-mapped I/O)
auto [vol, volErr] = vfs::openArchiveFileMmap("content.idx", "content.pak");
vfs::mount(fs, "/assets", std::move(vol), {.readOnly = true, .hashKeyLookup = true});

// Open and read a file
auto [file, openErr] = vfs::open(fs, "/assets/textures/pomdog.png");
auto [info, statErr] = file->stat();
std::vector<uint8_t> buffer(info.size);
auto [bytesRead, readErr] = file->read(std::span<uint8_t>(buffer));
```

### Mount Strategy

Applications typically configure VFS as follows:

1. **Archive mount** — Mount the packed archive at `/assets` with `hashKeyLookup = true`. This uses the sorted xxHash-64 key table in the `.idx` file for O(log n) lookup. The `.pak` file is memory-mapped for zero-copy reads.
2. **Overlay mount** — Optionally mount a loose-file directory at the same `/assets` path with `overlayFS = true`. When overlay is enabled, loose files take priority over archive entries. This allows developers to iterate on individual assets without rebuilding the archive.

### Hash-Based Lookup

When `hashKeyLookup` is enabled, the VFS can look up files by their xxHash-64 key pair (mount volume hash + file path hash) instead of string comparison. This is the fast path used in shipping builds.

### Loader Functions

All loader functions accept a `std::shared_ptr<vfs::FileSystemContext>` as their first parameter:

```cpp
// Textures
auto [tex, err] = loadTexture2D(fs, graphicsDevice, "/assets/textures/pomdog.png");

// Shaders
auto [shader, err] = loadShaderAutomagically(fs, graphicsDevice, ...);

// Fonts
auto [font, err] = loadTrueTypeFont(fs, "/assets/fonts/NotoSans-Regular.ttf");

// Models
auto [glb, err] = GLTF::Open(fs, "/assets/glb/f15.glb");
auto [vox, err] = loadVoxModel(fs, "/assets/voxel_models/maidchan.vox");

// Spine skeletal data
auto [atlas, err] = createTextureAtlas(fs, graphicsDevice,
    "/assets/skeletal2d/skeleton.tileset",
    "/assets/skeletal2d/skeleton.png");
auto [desc, err]  = spine::loadSkeletonDesc(fs, "/assets/skeletal2d/skeleton.json");

// SVG images
auto [tex, err] = loadTextureFromSVGFile(fs, graphicsDevice, "/assets/svg/icon.svg", 24, 24);

// Audio
auto [audio, err] = loadAudioClip(fs, audioEngine, "/assets/sounds/pong1.wav");

// Particles
auto [clip, err] = loadParticleClip(fs, "/assets/particles/fire2d.json");
```

## Build Tools Reference

All build tools are Go programs in `tools/cmd/`:

| Tool | Purpose |
|------|---------|
| `shader-ninja-gen` | Generates Ninja build files from `shaderbuild.toml` |
| `shader-archive-gen` | Converts `shaderbuild.toml` into archive recipe TOML |
| `spirv-rename-blocks` | Strips `_std140` suffix from UBO type names in SPIR-V |
| `spirv-patch-interface` | Restores dead-code eliminated PS input variables in SPIR-V |
| `spirv-strip-debug` | Strips debug and non-semantic instructions from SPIR-V |
| `spirv-shader-reflect` | Extracts reflection data (bindings) from SPIR-V |
| `glsl-rename-combined-samplers` | Rewrites `SPIRV_Cross_Combined*` sampler names back to original texture names |
| `spirv-link-validate` | Validates VS/PS interface compatibility |
| `archive-content` | Packs files into `.idx` + `.pak` archives |
| `archive-inspect` | Dumps archive contents for debugging |
| `bootstrap-toolchain` | Downloads and sets up the build toolchain |
| `glsl-minifier` | Minifies GLSL source for shipping |

Shared packages live in `tools/pkg/`:

| Package | Purpose |
|---------|---------|
| `archives` | Archive recipe parsing and generation |
| `ninja` | Ninja build file writer |
| `stringhash` | xxHash-32/64 with fixed seeds |
| `spirvreflect` | SPIR-V reflection data structures |
| `spvparse` | SPIR-V binary parser |
| `depfile` | Makefile-style dependency file parser |
