# spine-json-to-skeletal2d

A tool that converts a skeleton that [Spine](https://esotericsoftware.com/)
exported as JSON into the FlatBuffers files that Pomdog's 2D skeletal animation
runtime loads.

## Background

The runtime does not parse JSON. This tool resolves everything that the JSON
refers to by name, so that the runtime only verifies the files and reads them in
place:

- Bones become joints in an array, and parents, slots, mesh vertices, and
  animation tracks refer to them by index.
- Slot, skin, animation, and attachment names become XXH32 hashes (seed
  `20160723`). Skins and clips are sorted by hash for binary search, and an
  attachment hash is also the key of its region in the `.tileset` that
  [libgdx-atlas-to-tileset](../libgdx-atlas-to-tileset/README.md) writes.
- Angles are converted to radians within [-pi, pi].
- Keys are sorted by time.
- The bones of a mesh vertex are sorted by weight, and the four with the
  highest weights are kept.

One source produces three files, one for each kind of data:

| File | Schema | Content |
|------|--------|---------|
| `*.skeleton_skel2d` | `schemas/skeletal2d_skeleton.fbs` | Joint hierarchy and bind pose |
| `*.skin_skel2d` | `schemas/skeletal2d_skin.fbs` | For each skin, the slots in draw order with the attachments and meshes that they show in the setup pose |
| `*.clip_skel2d` | `schemas/skeletal2d_clip.fbs` | Animation clips |

The skin and clip files refer to joints by index, so load them with the
skeleton file of the same conversion.

The input is the JSON format of Spine 2.x, whose weighted meshes have the
`skinnedmesh` type. Other versions are not supported yet. The tool fails on
input that it cannot convert correctly, such as a setup attachment of another
type, an unknown bone or slot, a malformed mesh, or a key value outside the
ranges in `schemas/skeletal2d_clip.fbs`.

The runtime does not support everything that the format describes. The tool
drops such data and lists what it dropped in the `notes` of the debug file:
stepped and Bezier curves (which play as linear ones), non-uniform scale, slot
colors, keys that hide an attachment, and other timelines and sections such as
`events` and `ik`.

## Installation

```sh
cd tools/cmd/spine-json-to-skeletal2d
go build -o ../../../build/tools/spine-json-to-skeletal2d
```

The tool imports the Go code generated from the schemas, so run the bootstrap
or `tools/script/build_tools.sh` after changing a schema.

## Usage

```sh
spine-json-to-skeletal2d \
    -i <input.json> \
    --out-skeleton <output.skeleton_skel2d> \
    --out-skin <output.skin_skel2d> \
    --out-clip <output.clip_skel2d> \
    [--out-debug <output.skel2d-debug>]
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input Spine JSON file (required) |
| `--out-skeleton <path>` | Output skeleton file (required) |
| `--out-skin <path>` | Output skin file (required) |
| `--out-clip <path>` | Output clip file (required) |
| `--out-debug <path>` | Output debug file, which is not a runtime asset |

The debug file (`schemas/skeletal2d_debug.fbs`) maps joint indices and hashes
back to names, and holds the conversion notes. Print it with `flatc`:

```sh
flatc --json --strict-json --raw-binary -o ./build/tmp schemas/skeletal2d_debug.fbs -- <output.skel2d-debug>
```

### Example

```sh
./build/tools/spine-json-to-skeletal2d \
    -i ./examples/feature_showcase/assets/skeletal2d/maidgun/maidgun.json \
    --out-skeleton ./build/content/skeletal2d/maidgun/maidgun.skeleton_skel2d \
    --out-skin ./build/content/skeletal2d/maidgun/maidgun.skin_skel2d \
    --out-clip ./build/content/skeletal2d/maidgun/maidgun.clip_skel2d \
    --out-debug ./build/convertbuild/skeletal2d/maidgun/maidgun.skel2d-debug
```

## Build pipeline integration

`asset-convert-ninja-gen` generates a Ninja build rule for each
`[[skeletal2d_from_spine_json]]` entry of a recipe:

```toml
[[skeletal2d_from_spine_json]]
in = "skeletal2d/maidgun/maidgun.json"
out_skeleton = "skeletal2d/maidgun/maidgun.skeleton_skel2d"
out_skin = "skeletal2d/maidgun/maidgun.skin_skel2d"
out_clip = "skeletal2d/maidgun/maidgun.clip_skel2d"
```

See [asset-convert-ninja-gen](../asset-convert-ninja-gen/README.md) for the
recipe format.

## Runtime loading

```cpp
auto [skeletonData, skeletonErr] = skeletal2d::loadSkeletonData(fs, "/assets/skeletal2d/maidgun/maidgun.skeleton_skel2d");
auto [skinData, skinErr] = skeletal2d::loadSkinData(fs, "/assets/skeletal2d/maidgun/maidgun.skin_skel2d");
auto [clipData, clipErr] = skeletal2d::loadAnimationClipData(fs, "/assets/skeletal2d/maidgun/maidgun.clip_skel2d");

auto skeleton = skeletal2d::createSkeleton(*skeletonData);
auto [skin, createSkinErr] = skeletal2d::createSkin(*skinData, *textureAtlas, "default");
auto [clip, createClipErr] = skeletal2d::createAnimationClip(*clipData, textureAtlas, "Run");
```

The snippet omits error handling for brevity.
