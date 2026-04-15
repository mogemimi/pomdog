# quickstart

A project scaffolding tool that generates a new Pomdog game project from the `examples/quickstart` template. This is the recommended way to start a new game or application with the Pomdog engine.

## Background

The Pomdog engine ships with example projects under `examples/`, but manually copying and adapting one as a starting point is tedious and error-prone. The quickstart tool automates this process: it copies the template files, sets up the directory structure with build scripts, initializes a git repository, and adds the Pomdog engine as a git submodule — producing a ready-to-build project in seconds.

## Features

- **Interactive mode**: Prompts for configuration (for human developers)
- **Non-interactive mode**: Accepts all configuration via flags (for CI and automation)
- **Git integration**: Initializes a git repository and adds Pomdog as a submodule
- **Build scripts**: Generates platform-specific bootstrap, asset build, and packaging scripts
- **Name validation**: Enforces lowercase alphanumeric project names (with underscores)

## Prerequisites

- **Go**: Required to build the quickstart tool
- **Git**: Required for repository initialization and submodule setup

## Installation

```sh
cd tools/cmd/quickstart
go build -o ../../../build/tools/quickstart
```

## Usage

```sh
quickstart [options]
```

### Options

| Option | Description |
|--------|-------------|
| `-pomdog <path>` | Path to the Pomdog repository (for template files) |
| `-o <path>` | Parent directory for the new project (default: `.`) |
| `-name <name>` | Project name (lowercase letters, digits, underscores; must start with a letter) |
| `-url <identifier>` | Bundle identifier (default: `com.example.<name>`) |
| `-pomdog-url <url>` | Git URL for the Pomdog submodule (default: auto-detect from pomdog dir) |
| `-pomdog-ref <ref>` | Git ref (branch, tag, or commit) for the Pomdog submodule |
| `-non-interactive` | Non-interactive mode for CI and automation |
| `-skip-git` | Skip git init and submodule setup |
| `-verbose` | Enable verbose output |

### Examples

Interactive mode (human developer):

```sh
./build/tools/quickstart -pomdog ./pomdog
```

Non-interactive mode (CI / automation):

```sh
./build/tools/quickstart \
    -non-interactive \
    -pomdog ./pomdog \
    -name my_game \
    -o /path/to/projects
```

With a specific Pomdog version:

```sh
./build/tools/quickstart \
    -non-interactive \
    -pomdog ./pomdog \
    -pomdog-url https://github.com/mogemimi/pomdog.git \
    -pomdog-ref v0.1.0 \
    -name my_game
```

Skip git setup (for testing):

```sh
./build/tools/quickstart \
    -non-interactive \
    -pomdog ./pomdog \
    -name test_project \
    -skip-git
```

## Generated Project Structure

```
my_game/
├── .git/
├── .gitignore
├── CMakeLists.txt
├── README.md
├── assets/
│   ├── assetconvert.toml
│   ├── copybuild.toml
│   ├── pomdog.png
│   ├── archive/
│   │   └── archive.toml
│   └── shaders/
│       ├── shaderbuild.toml
│       └── simple_effect.slang
├── build/
├── platform/
│   ├── cocoa/
│   ├── emscripten/
│   ├── linux/
│   └── win32/
├── pomdog/                    # git submodule
├── source/
│   ├── game_main.cpp
│   └── game_main.h
└── tools/
    ├── bootstrap.toml
    ├── cmd/
    ├── pkg/
    └── script/
        ├── assetbuild.sh
        ├── bootstrap.sh
        ├── build_tools.sh
        ├── clang_format.sh
        ├── docs_format.sh
        ├── package_emscripten.sh
        ├── package_linux.sh
        └── package_windows.sh
```

## Project Name Rules

Project names must follow these rules:

- Only lowercase ASCII letters (`a-z`), digits (`0-9`), and underscores (`_`)
- Must start with a lowercase letter
- Examples: `my_game`, `hello`, `game2d`
- Invalid: `MyGame`, `_game`, `123game`, `my-game`
