# mesh3d-source-to-mesh3d

A tool that optimizes a mesh source, which
[gltf-to-mesh3d-source](../gltf-to-mesh3d-source/README.md) imports from a 3D
model, into the files that Pomdog's runtime loads into GPU buffers.

## Background

The tool writes three files for the runtime and one for debugging:

| File | Format | Content |
|------|--------|---------|
| `*.mesh3d` | FlatBuffers (`schemas/mesh3d.fbs`) | Meshes by name hash, the parts that each draw call draws, the vertex buffer layouts, and the index type |
| `*.mesh3dvertices` | Raw, no header | The vertex buffers one after another, as little-endian 32-bit floats |
| `*.mesh3dindices` | Raw, no header | The indices of every part, as little-endian 16-bit or 32-bit integers |
| `*.mesh3d-debug` | FlatBuffers (`schemas/mesh3d_debug.fbs`) | Mesh and material names, vertex counts before and after the optimization, and notes. Not a runtime asset. |

The `.mesh3d` file has no vertex data and no materials, so it stays small enough
to keep in memory. The two raw files go to the GPU as they are; the `.mesh3d`
file tells where each vertex buffer is and how its vertices are laid out.

To fill the buffers, the tool:

- Interleaves the attributes of a primitive into one vertex of floats, with the
  elements ordered by semantic: position, normal, tangent, texture
  coordinates, color, joints, and weights. Attributes of the same semantic are
  ordered by set index, such as `TEXCOORD_0` before `TEXCOORD_1`.
- Puts the primitives whose vertices have the same layout into one vertex
  buffer, in the order in which the meshes first use the layout.
- Merges vertices whose bytes are identical, and leaves out the vertices that
  no index refers to. A vertex gets its index in the order in which the
  triangles refer to it.
- Picks 16-bit indices when every vertex buffer has at most 65536 vertices, and
  32-bit indices otherwise. An index counts from the start of its vertex
  buffer.
- Sorts the meshes by the XXH32 hash (seed `20160723`) of their names for binary
  search, and fails on a mesh without a name, a duplicate name, or a hash
  collision.

The runtime draws lists of triangles, so the tool fails on a primitive of
another topology. It drops attributes whose names are not semantics of a vertex
element and lists them in the `notes` of the debug file, along with the notes
of the mesh source.

The same input always gives the same bytes.

## Installation

```sh
cd tools/cmd/mesh3d-source-to-mesh3d
go build -o ../../../build/tools/mesh3d-source-to-mesh3d
```

The tool imports the Go code generated from the schemas, so run the bootstrap
or `tools/script/build_tools.sh` after changing a schema.

## Usage

```sh
mesh3d-source-to-mesh3d \
    -i <input.mesh3dsource> \
    --out-mesh <output.mesh3d> \
    --out-vertices <output.mesh3dvertices> \
    --out-indices <output.mesh3dindices> \
    [--out-debug <output.mesh3d-debug>]
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input mesh source file (required) |
| `--out-mesh <path>` | Output mesh file (required) |
| `--out-vertices <path>` | Output vertices file (required) |
| `--out-indices <path>` | Output indices file (required) |
| `--out-debug <path>` | Output debug file, which is not a runtime asset |

Print the mesh file or the debug file with `flatc`:

```sh
flatc --json --strict-json --raw-binary -o ./build/tmp schemas/mesh3d_debug.fbs -- <output.mesh3d-debug>
```

### Example

```sh
./build/tools/mesh3d-source-to-mesh3d \
    -i ./build/convertbuild/glb/f15.mesh3dsource \
    --out-mesh ./build/content/glb/f15.mesh3d \
    --out-vertices ./build/content/glb/f15.mesh3dvertices \
    --out-indices ./build/content/glb/f15.mesh3dindices \
    --out-debug ./build/convertbuild/glb/f15.mesh3d-debug
```

## Build pipeline integration

`asset-convert-ninja-gen` runs `gltf-to-mesh3d-source` and then this tool for
each `[[mesh3d_from_gltf]]` entry of a recipe:

```toml
[[mesh3d_from_gltf]]
in = "glb/f15.glb"
out_mesh = "glb/f15.mesh3d"
out_vertices = "glb/f15.mesh3dvertices"
out_indices = "glb/f15.mesh3dindices"
```

See [asset-convert-ninja-gen](../asset-convert-ninja-gen/README.md) for the
recipe format.

## Runtime loading

```cpp
auto [meshData, meshErr] = mesh3d::loadMeshData(fs, "/assets/glb/f15.mesh3d");
auto [buffers, buffersErr] = mesh3d::loadMeshBuffers(fs, graphicsDevice, *meshData,
    "/assets/glb/f15.mesh3dvertices",
    "/assets/glb/f15.mesh3dindices");

const auto mesh = meshData->findMeshByKey(computeStringHash32("Cube"));
for (u32 i = 0; i < meshData->getPartCount(*mesh); ++i) {
    const auto part = meshData->getPart(*mesh, i);
    commandList->setVertexBuffer(0, buffers.vertexBuffers[part.vertexBuffer]);
    commandList->setIndexBuffer(buffers.indexBuffer);
    commandList->drawIndexed(part.indexCount, part.startIndex);
}
```

The snippet omits error handling for brevity.
