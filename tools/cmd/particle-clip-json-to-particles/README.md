# particle-clip-json-to-particles

A tool that converts a particle clip written in JSON into the FlatBuffers file
that `loadParticleClip()` reads.

## Background

The runtime does not parse JSON. This tool checks a particle clip and converts
it, so that the runtime only verifies the file and creates the clip:

- Angles become radians: `start_rotation`, `rotation_over_lifetime`, and the
  `angle` of a sector. The conversion computes in float32, as
  `math::toRadian(f32)` does, so the runtime gets the same angles as when it
  parsed the JSON itself.
- The points of a curve are sorted by time.
- The ranges that `ParticleSystem` relies on are checked during the build, such
  as positive emission rates and at most 4096 particles.

The output is a `.particlesclip` file (`schemas/particles_clip.fbs`). The
format keeps the data model of the JSON source; an editor and a new format are
planned separately.

## Source format

```json
{
  "duration": 0.5,
  "emission_rate": 450,
  "emission_rate_over_time": 100,
  "start_lifetime": 1.0,
  "start_delay": 0.0,
  "gravity_modifier": 980.0,
  "max_particles": 1024,
  "looping": true,
  "shape": {"type": "sector", "angle": 60.0},
  "start_speed": {"type": "random", "min": 100.0, "max": 500.0},
  "start_color": {"type": "constant", "value": [10, 120, 190, 0]},
  "color_over_lifetime": {
    "type": "curve",
    "points": [{"t": 0.0, "v": [130, 220, 190, 0]}, {"t": 1.0, "v": [0, 0, 0, 0]}]
  },
  "start_rotation": {"type": "random", "min": 0.0, "max": 360.0},
  "rotation_over_lifetime": {"type": "random", "min": -45.0, "max": 45.0},
  "start_size": {"type": "random", "min": 0.1, "max": 0.3},
  "size_over_lifetime": {
    "type": "curve",
    "points": [{"t": 0.0, "v": 0.0}, {"t": 0.15, "v": 1.0}, {"t": 1.0, "v": 0.0}]
  }
}
```

Every field is required.

| Field | Description |
|-------|-------------|
| `duration` | Seconds that the clip emits for, greater than zero |
| `emission_rate`, `emission_rate_over_time` | Integers greater than zero |
| `max_particles` | Integer within [1, 4096] |
| `start_lifetime` | Seconds that a particle lives, greater than zero |
| `start_delay`, `gravity_modifier` | Numbers |
| `looping` | Boolean |
| `shape` | Emitter shape, see below |
| `start_speed`, `start_size`, `size_over_lifetime` | Number parameters |
| `start_rotation`, `rotation_over_lifetime` | Number parameters in degrees |
| `start_color`, `color_over_lifetime` | Color parameters |

A shape has a `type` and the fields of that type, which are never negative:

| `type` | Fields |
|--------|--------|
| `box` | `width`, `height`, and optionally `depth` (default 0) |
| `cone` | `radius`, `height` |
| `hemisphere`, `sphere` | `radius` |
| `sector` | `angle`, the central angle within [0, 360] degrees |

A parameter has a `type` and the fields of that type. Its values are numbers,
or arrays of four integer channels within [0, 255] for colors.

| `type` | Fields |
|--------|--------|
| `constant` | `value` |
| `random` | `min`, `max` |
| `curve` | `points`: at least one `{"t": time, "v": value}`, where `t` is the normalized lifetime within [0, 1] |

The tool fails on an unknown field, a missing field, a field that the type does
not use, or a value out of its range.

## Installation

```sh
cd tools/cmd/particle-clip-json-to-particles
go build -o ../../../build/tools/particle-clip-json-to-particles
```

The tool imports the Go code generated from the schemas, so run the bootstrap
or `tools/script/build_tools.sh` after changing a schema.

## Usage

```sh
particle-clip-json-to-particles -i <input.json> -o <output.particlesclip>
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input particle clip JSON file (required) |
| `-o <path>` | Output particle clip file (required) |

Print the output with `flatc`:

```sh
flatc --json --strict-json --raw-binary -o ./build/tmp schemas/particles_clip.fbs -- <output.particlesclip>
```

### Example

```sh
./build/tools/particle-clip-json-to-particles \
    -i ./examples/feature_showcase/assets/particles/fire2d.json \
    -o ./build/content/particles/fire2d.particlesclip
```

## Build pipeline integration

`asset-convert-ninja-gen` generates a Ninja build rule for each
`[[particles_clip_from_json]]` entry of a recipe:

```toml
[[particles_clip_from_json]]
in = "particles/fire2d.json"
out = "particles/fire2d.particlesclip"
```

See [asset-convert-ninja-gen](../asset-convert-ninja-gen/README.md) for the
recipe format.

## Runtime loading

```cpp
auto [clip, err] = loadParticleClip(fs, "/assets/particles/fire2d.particlesclip");
auto particleSystem = std::make_unique<ParticleSystem>(clip);
```

The snippet omits error handling for brevity.
