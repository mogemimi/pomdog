# subninja-gen

A code generator that produces a top-level [Ninja](https://ninja-build.org/) build file combining multiple sub-build files via the `subninja` directive.

The Pomdog asset pipeline consists of several independent Ninja build phases — shader compilation (`shader-ninja-gen`), file copying (`copy-ninja-gen`), and archive packaging (`archive-ninja-gen`). Each phase produces its own `.ninja` file. Previously, these were executed as separate `ninja` invocations from the shell script, which prevented Ninja from parallelising across phases or tracking cross-phase dependencies. By combining all sub-builds into a single top-level `build.ninja` with `subninja`, a single `ninja` invocation can run all phases with unified parallelism, correct dependency ordering, and shared build state.

## Features

- **Unified execution**: Merges multiple Ninja build files into one so a single `ninja` call builds everything
- **Cross-phase parallelism**: Ninja can run independent build edges from different phases concurrently
- **Dependency tracking**: Build edges in one sub-ninja can depend on outputs from another, and Ninja resolves the ordering automatically
- **Simple interface**: Takes a list of `.ninja` file paths as positional arguments

## Installation

```sh
cd tools/cmd/subninja-gen
go build -o ../../../build/tools/subninja-gen
```

## Usage

```sh
subninja-gen [options] <file1.ninja> [file2.ninja ...]
```

### Options

| Option | Description |
|--------|-------------|
| `-o <path>` | Output Ninja build file path |

Positional arguments are paths to `.ninja` files to include as subninjas.

### Examples

Combine shader, copy, and archive builds for the quickstart example:

```sh
./build/tools/subninja-gen \
    -o ./build/quickstart/build.ninja \
    ./build/quickstart/shaderbuild/shaders_pomdog.ninja \
    ./build/quickstart/shaderbuild/shaders_app.ninja \
    ./build/quickstart/copybuild/copy_pomdog.ninja \
    ./build/quickstart/copybuild/copy_app.ninja \
    ./build/quickstart/archivebuild/build.ninja
```

Then run the entire pipeline with:

```sh
ninja -C build/quickstart
```

## Debugging Incremental Builds

When investigating why a particular build step keeps running even though nothing appears to have changed, use Ninja's dry-run + explain mode:

```sh
ninja -C build/quickstart -n -d explain
```

| Flag | Effect |
|------|--------|
| `-n` | Dry run — prints commands that would execute without actually running them |
| `-d explain` | Prints the reason each build edge is considered out-of-date (e.g. "input X is newer than output Y") |

This is especially useful when an upstream generator (e.g. `shader-archive-gen`) is invoked outside the Ninja build graph, causing files to be re-written with a new timestamp even when their content hasn't changed.

### Example output

```
ninja explain: output build/quickstart/archive/build/archive_shaders_pomdog.toml older than most recent input build/quickstart/archive/build/archive_shaders_pomdog.toml (1750217420 vs 1750217426)
```

In this case the fix is to move the generator invocation into a Ninja build rule so that Ninja can track the dependency correctly and skip the step when the input hasn't changed.

## Generated Output

The generated `build.ninja` contains only `subninja` directives:

```ninja
subninja build/quickstart/shaderbuild/shaders_pomdog.ninja
subninja build/quickstart/shaderbuild/shaders_app.ninja
subninja build/quickstart/copybuild/copy_pomdog.ninja
subninja build/quickstart/copybuild/copy_app.ninja
subninja build/quickstart/archivebuild/build.ninja
```
