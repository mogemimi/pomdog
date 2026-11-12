# voxelmodel-to-mesh3d-source

A tool that turns the grids of a voxel model (`.voxelmodel`) into polygon
meshes, written as a mesh source that
[mesh3d-source-to-mesh3d](../mesh3d-source-to-mesh3d/README.md) optimizes for
the runtime, as it does for a glTF model.

## Background

A MagicaVoxel file becomes meshes in three steps:

1. [vox-to-voxelmodel](../vox-to-voxelmodel/README.md) converts it into a
   voxel model.
2. This tool turns the voxel model into a mesh source
   (`schemas/mesh3d_source.fbs`).
3. `mesh3d-source-to-mesh3d` lays the vertices out for the GPU.

Each grid becomes a mesh named `model0`, `model1`, and so on after its index,
with one primitive of triangles whose vertices have a `POSITION`, a `NORMAL`,
and a `COLOR_0`, which BasicEffect draws with its position, normal, and color
variant. The color is the palette color of the voxel divided by 255, as stored
in the palette. A grid without voxels becomes a mesh without primitives.

The vertices use the coordinate system of glTF: MagicaVoxel points z up, and
the tool turns (x, y, z) into (x, z, -y), which points y up and keeps the
system right-handed. One voxel is one unit, and the origin of a mesh is the
center of its grid, which MagicaVoxel places at half the size of the grid,
rounded down. The triangles are counterclockwise seen from outside.

The scene graph, when the voxel model has one, becomes the nodes of the mesh
source: each transform becomes a node, which takes the transforms of a group
as its children or the mesh of a shape. The first keyframe places the node, and
a rotation that mirrors becomes a rotation with a scale of -1 on z. The runtime
mesh file does not keep nodes, so the meshes of a scene stand at their own
origins. The notes of the mesh source list hidden and animated nodes, and the
materials of MagicaVoxel, which the tool does not convert.

## Installation

```sh
cd tools/cmd/voxelmodel-to-mesh3d-source
go build -o ../../../build/tools/voxelmodel-to-mesh3d-source
```

## Usage

```sh
voxelmodel-to-mesh3d-source -i <input.voxelmodel> -o <output.mesh3dsource> [-mesher <name>]
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input voxel model file (required) |
| `-o <path>` | Output mesh source file (required) |
| `-mesher <name>` | Faces that become polygons (default: `greedy`) |

The meshers:

| Mesher | Faces |
|--------|-------|
| `naive` | Every face of every voxel |
| `culled` | The faces that no other voxel of the grid covers |
| `greedy` | The faces of `culled`, merged into rectangles of one color on each plane |

`greedy` makes the fewest triangles. Its rectangles can meet in T-junctions,
where a rasterizer may leave gaps of a pixel; `culled` has none.

### Example

```sh
./build/tools/voxelmodel-to-mesh3d-source \
    -i ./build/feature_showcase/convertbuild/voxel_models/maidchan.voxelmodel \
    -o ./build/tmp/maidchan.mesh3dsource
```

## Build pipeline integration

`asset-convert-ninja-gen` runs `vox-to-voxelmodel`, this tool, and then
`mesh3d-source-to-mesh3d` for each `[[mesh3d_from_vox]]` entry of a recipe. See
[asset-convert-ninja-gen](../asset-convert-ninja-gen/README.md) for the recipe
format.
