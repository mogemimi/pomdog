# zstd-wrapper

A thin wrapper around the Zstandard command-line tool (`zstd`) that gives the output file the time when zstd wrote it as its modification time.

## Background

When `zstd` compresses a file into another file, it copies the modification time of the input file to the output file, and no zstd option turns this off. Build tools such as Ninja rebuild an output that is older than its inputs, so a frame that keeps the time of its input looks out of date as soon as another input, such as a dictionary, is newer.

`zstd-wrapper` solves this by:

1. Running `zstd` with all arguments passed through unchanged
2. Forwarding stdout and stderr as-is
3. Setting the modification time of the file after `-o` to the current time when zstd succeeds
4. Preserving the exit code of `zstd`

## Installation

```sh
cd tools/cmd/zstd-wrapper
go build -o ../../../build/tools/zstd-wrapper
```

## Usage

```sh
zstd-wrapper <path-to-zstd> [zstd options...] -o <output> [inputs...]
```

The first argument is the path to the `zstd` executable. All remaining arguments are forwarded to `zstd` unchanged, and they must contain exactly one `-o <output>`.

### Examples

Compress a file at level 19:

```sh
./build/tools/zstd-wrapper ./build/tools/zstd -q -f -19 input.json -o input.json.zst
```

In the asset build, `archive-ninja-gen` generates build rules that invoke `zstd-wrapper` instead of `zstd` directly:

```ninja
rule zstd_compress
  command = $zstd_wrapper_exe $zstd_exe -q -f $zstd_level $in -o $out
```

## Behavior

| Scenario | Behavior |
|----------|----------|
| zstd succeeds | Sets the modification time of the output to the current time. |
| zstd fails | Leaves the output as zstd left it. Exits with zstd's exit code. |
| No `-o`, or more than one `-o` | Fails without running zstd. |
