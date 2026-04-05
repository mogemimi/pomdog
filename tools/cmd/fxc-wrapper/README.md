# fxc-wrapper

A thin wrapper around Microsoft's `fxc.exe` (the legacy HLSL compiler for Direct3D 11 / Shader Model 4.0) that suppresses the verbose success message emitted on every successful compilation.

## Background

When `fxc.exe` compiles a shader successfully, it always prints a line like:

```
compilation object save succeeded; see C:\path\to\output.dxbc
```

This message is written to stdout and cannot be suppressed by any fxc option (including `/nologo`). In a Ninja-based build pipeline, this creates noisy output — Ninja expects tools to be silent on success and only produce output on errors or warnings.

`fxc-wrapper` solves this by:

1. Running `fxc.exe` with all arguments passed through unchanged
2. Capturing stdout and filtering out lines that start with `compilation object save succeeded`
3. Forwarding stderr as-is (which contains error and warning messages)
4. Preserving the exit code of `fxc.exe`

## Installation

```sh
cd tools/cmd/fxc-wrapper
go build -o ../../../build/tools/fxc-wrapper.exe
```

## Usage

```sh
fxc-wrapper <path-to-fxc.exe> [fxc options...]
```

The first argument is the path to the actual `fxc.exe` binary. All remaining arguments are forwarded to `fxc.exe` unchanged.

### Options

All options after the first argument are passed directly to `fxc.exe`. See [Microsoft's fxc documentation](https://learn.microsoft.com/en-us/windows/win32/direct3dtools/fxc) for available options.

### Examples

Compile an HLSL shader to DXBC (SM 4.0):

```sh
./build/tools/fxc-wrapper "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/fxc.exe" \
    /nologo /T ps_4_0 /E main /Fo output.dxbc input.hlsl
```

In the Ninja build pipeline, `shader-ninja-gen` generates build rules that invoke `fxc-wrapper` instead of `fxc.exe` directly:

```ninja
rule compile_dxbc
  command = $fxc_wrapper_exe $fxc_exe /nologo /T $stage /E $entrypoint /Fo $out $in
```

## Behavior

| Scenario | Behavior |
|----------|----------|
| fxc succeeds | Strips `compilation object save succeeded; see ...` from stdout. Produces no output. |
| fxc fails | Forwards stderr (error messages) as-is. Exits with fxc's exit code. |
| fxc emits warnings | Forwards stderr (warnings) as-is. Strips success message from stdout. |
