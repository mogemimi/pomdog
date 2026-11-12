# vox-to-voxelmodel

A tool that converts a [MagicaVoxel](https://ephtracy.github.io/) file (`.vox`)
to a voxel model (`.voxelmodel`), the FlatBuffers voxel data of Pomdog.
[voxelmodel-to-vox](../voxelmodel-to-vox/README.md) converts it back.

## Background

The runtime does not parse `.vox` files. A voxel model holds the same data in
FlatBuffers (`schemas/voxel_model.fbs`), so that an editor or a tool can make or
change voxel data without knowing the `.vox` format, and `voxelmodel-to-vox`
can still turn the result into a file that MagicaVoxel opens. The runtime reads
and writes voxel models with `pomdog/experimental/voxelmodel`, and
[voxelmodel-to-mesh3d-source](../voxelmodel-to-mesh3d-source/README.md) turns
them into meshes.

The tool reads both versions of the format:

- Version 150, which the format documentation describes. The files of older
  releases, such as the sample of `examples/feature_showcase`, have only
  `SIZE`, `XYZI`, and `RGBA` chunks, sometimes with a `PACK` chunk for the
  frames of an animation, and use the default palette of MagicaVoxel when they
  have no `RGBA` chunk.
- Version 200, which recent releases write. Recent releases add the scene graph
  (`nTRN`, `nGRP`, and `nSHP`), layers (`LAYR`), and materials (`MATL`) to
  either version.

A voxel model keeps the grids (the models of MagicaVoxel), the palette, the
materials, the layers, and the scene graph. The `_name`, `_hidden`, `_r`, `_t`,
and `_f` entries of the dictionaries get their own fields, and the other
entries are kept as text. The nodes are numbered in the order of their IDs, so
that the root, whose ID is 0, comes first.

The tool drops the chunks that a voxel model does not keep and lists them in
its `notes`: render settings (`rOBJ`), cameras (`rCAM`), names of palette rows
(`NOTE`), the display order of the palette (`IMAP`), settings of the document
such as the range of the animation (`META`), the materials of the old format
(`MATT`), and unknown chunks.

The tool fails on a file that it cannot convert correctly, such as a grid
larger than 256 voxels, a voxel outside its grid or with the color index 0,
two voxels at one position, or a scene graph that is not a tree.

## Installation

```sh
cd tools/cmd/vox-to-voxelmodel
go build -o ../../../build/tools/vox-to-voxelmodel
```

The tool imports the Go code generated from the schemas, so run the bootstrap
or `tools/script/build_tools.sh` after changing a schema.

## Usage

```sh
vox-to-voxelmodel -i <input.vox> -o <output.voxelmodel> [--drop-unused-materials] [--drop-unused-layers]
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input MagicaVoxel file (required) |
| `-o <path>` | Output voxel model file (required) |
| `--drop-unused-materials` | Drop the materials of the colors that no voxel uses |
| `--drop-unused-layers` | Drop the layers that no transform uses |

Print a voxel model with `flatc`:

```sh
flatc --json --strict-json --raw-binary -o ./build/tmp schemas/voxel_model.fbs -- <output.voxelmodel>
```

### Example

```sh
./build/tools/vox-to-voxelmodel \
    -i ./examples/feature_showcase/assets/voxel_models/maidchan.vox \
    -o ./build/convertbuild/voxel_models/maidchan.voxelmodel
```

### Reducing the size

Recent releases of MagicaVoxel write a material for each of the 256 colors and
all 16 layers, even when they keep their defaults. A model that uses a few
colors then spends most of its file on materials: 20 KB of a 28 KB file. The
two `--drop-unused-*` options leave out the materials and the layers that the
model does not use, which MagicaVoxel replaces with its defaults when it opens
the file, and list what they drop in the `notes`.

The materials that remain still repeat the same keys and values, such as
`_rough` and `0.1`. The tool writes the keys and the values of all dictionaries
as shared strings, which a FlatBuffers buffer stores once however many times
they appear. `tools/cmd/bootstrap-toolchain/bootstrap.toml` turns them on for
`VoxelModelAttribute.go`, the Go code that `flatc` generates for these entries,
through the `shared_string` option of
[flatbuffers-go-modify](../flatbuffers-go-modify/README.md), and
`voxelmodel::saveVoxelModel()` writes them the same way.

Converting the result back with
[voxelmodel-to-vox](../voxelmodel-to-vox/README.md) shrinks the `.vox` file
itself:

```sh
./build/tools/vox-to-voxelmodel -i model.vox -o ./build/tmp/model.voxelmodel \
    --drop-unused-materials --drop-unused-layers
./build/tools/voxelmodel-to-vox -i ./build/tmp/model.voxelmodel -o model.vox
```

The new file also leaves out the chunks that a voxel model does not keep,
such as cameras and render settings.

## Build pipeline integration

`asset-convert-ninja-gen` runs this tool for each `[[voxelmodel_from_vox]]`
entry of a recipe, and before `voxelmodel-to-mesh3d-source` for each
`[[mesh3d_from_vox]]` entry. See
[asset-convert-ninja-gen](../asset-convert-ninja-gen/README.md) for the recipe
format.
