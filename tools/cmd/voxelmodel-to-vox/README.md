# voxelmodel-to-vox

A tool that converts a voxel model (`.voxelmodel`) to a
[MagicaVoxel](https://ephtracy.github.io/) file (`.vox`), so that voxel data
made or edited by a tool or by the runtime opens in MagicaVoxel.
[vox-to-voxelmodel](../vox-to-voxelmodel/README.md) converts the other way.

## Background

The tool writes version 150 of the format, which both old and recent releases
of MagicaVoxel read:

- A `SIZE` and an `XYZI` chunk for each grid, in the order of the grids.
- The scene graph, when the voxel model has one, as `nTRN`, `nGRP`, and `nSHP`
  chunks whose node IDs are the indices of the nodes. A file with more than one
  grid and no scene graph gets a `PACK` chunk, which old releases read as the
  frames of an animation.
- The layers (`LAYR`), the palette (`RGBA`, always written), and the materials
  (`MATL`).

The `_name`, `_hidden`, `_r`, `_t`, and `_f` entries are written only when they
differ from what MagicaVoxel assumes without them, followed by the other
entries of each dictionary. A voxel model that `vox-to-voxelmodel` wrote
therefore converts back to the same voxel model, except for its `notes`, which
a `.vox` file has no place for.

The tool fails on a voxel model that breaks a rule of `schemas/voxel_model.fbs`.

## Installation

```sh
cd tools/cmd/voxelmodel-to-vox
go build -o ../../../build/tools/voxelmodel-to-vox
```

## Usage

```sh
voxelmodel-to-vox -i <input.voxelmodel> -o <output.vox>
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input voxel model file (required) |
| `-o <path>` | Output MagicaVoxel file (required) |

### Example

```sh
./build/tools/voxelmodel-to-vox \
    -i ./build/feature_showcase/content/voxel_models/maidchan.voxelmodel \
    -o ./build/tmp/maidchan.vox
```
