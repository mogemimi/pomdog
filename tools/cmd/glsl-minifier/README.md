# glsl-minifier

A lightweight GLSL source minifier that reduces shader file size for shipping builds. The shader compilation pipeline (shader-ninja-gen) uses this tool to minify transpiled GLSL output before packaging, reducing download size and memory usage on desktop OpenGL and WebGL/Emscripten targets.

## Features

- **Comment removal**: Strips both multi-line (`/* */`) and single-line (`//`) comments
- **Whitespace reduction**: Collapses redundant whitespace and removes empty lines
- **Operator compaction**: Removes unnecessary spaces around operators and braces
- **Preprocessor preservation**: Keeps `#`-prefixed lines on separate lines as required by GLSL

## Installation

```sh
cd tools/cmd/glsl-minifier
go build -o ../../../build/tools/glsl-minifier
```

## Usage

```sh
glsl-minifier -i <input.glsl> -o <output.glsl>
```

### Options

| Option | Required | Description |
|--------|----------|-------------|
| `-i <path>` | Yes | Path to input GLSL file |
| `-o <path>` | Yes | Path to output minified GLSL file |

### Examples

Minify a single GLSL file:

```sh
./build/tools/glsl-minifier \
    -i build/shaderbuild/glsl_es300/sprite_batch_vs.glsl \
    -o build/shaderbuild/glsl_es300/sprite_batch_vs.minified.glsl
```

### Notes

- The tool operates on text-level transformations only; it does not parse the GLSL AST.
- Input files are typically spirv-cross transpiled GLSL, not hand-written source.
