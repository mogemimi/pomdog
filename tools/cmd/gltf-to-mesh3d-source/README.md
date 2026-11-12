# gltf-to-mesh3d-source

A tool that imports the meshes of a [glTF](https://www.khronos.org/gltf/) 2.0
binary file (`.glb`) into a mesh source, the FlatBuffers file that
[mesh3d-source-to-mesh3d](../mesh3d-source-to-mesh3d/README.md) optimizes for
the runtime.

## Background

The runtime does not parse glTF. Pomdog converts a 3D model in two steps:

1. This tool imports the model into a mesh source (`schemas/mesh3d_source.fbs`).
2. `mesh3d-source-to-mesh3d` lays the vertices out for the GPU and writes the
   files that the runtime loads.

The mesh source is build intermediate data, which a game does not ship. It is
unoptimized and meant for tools: the vertex data is decoded into arrays, so the
tools that read it do not depend on glTF, and a tool for another source format
can write the same table. It keeps what the source says about the model:

- Each primitive of a mesh has its attributes as arrays of floats, named as in
  glTF (`POSITION`, `NORMAL`, `TEXCOORD_0`, and so on) and sorted by name, and
  its indices as an array of 32-bit integers.
- The tool resolves accessors, buffer views, byte strides, and byte offsets. An
  integer component becomes a float, in [0, 1] or [-1, 1] when the accessor
  normalizes it, and the attribute records the type that it had.
- Mesh names, materials (the factors of the metallic-roughness model), and the
  node hierarchy and scenes are copied with the glTF defaults filled in.

Data that the mesh source cannot hold yet is dropped and listed in its `notes`:
morph targets, animations, cameras, images, samplers, skins, and textures.

The tool fails on input that it cannot import correctly, such as an external
buffer file, a sparse accessor, a matrix accessor, an extension that the file
requires, or an index or reference that is out of range.

## Installation

```sh
cd tools/cmd/gltf-to-mesh3d-source
go build -o ../../../build/tools/gltf-to-mesh3d-source
```

The tool imports the Go code generated from the schemas, so run the bootstrap
or `tools/script/build_tools.sh` after changing a schema.

## Usage

```sh
gltf-to-mesh3d-source -i <input.glb> -o <output.mesh3dsource>
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input glTF binary file (required) |
| `-o <path>` | Output mesh source file (required) |

Print a mesh source with `flatc`:

```sh
flatc --json --strict-json --raw-binary -o ./build/tmp schemas/mesh3d_source.fbs -- <output.mesh3dsource>
```

### Example

```sh
./build/tools/gltf-to-mesh3d-source \
    -i ./examples/feature_showcase/assets/glb/f15.glb \
    -o ./build/convertbuild/glb/f15.mesh3dsource
```

## Build pipeline integration

`asset-convert-ninja-gen` runs this tool and then `mesh3d-source-to-mesh3d` for
each `[[mesh3d_from_gltf]]` entry of a recipe. See
[asset-convert-ninja-gen](../asset-convert-ninja-gen/README.md) for the recipe
format.
