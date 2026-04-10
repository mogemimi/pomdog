# wav-crossfade-conv

A Go CLI tool that processes WAV audio files for seamless looping by applying crossfade blending between the start and end of the audio data. This is essential for creating loop-friendly ambient sounds and background music without audible clicks or pops at the loop boundary.

## How It Works

1. **Read** the input WAV file into memory
2. **Align** the sample count to a given alignment boundary (optional)
3. **Crossfade** the tail of the audio into the head using the chosen fade curve
4. **Trim** the loop-start portion of the crossfade region (optional)
5. **Write** the processed audio to a new WAV file

The crossfade works by blending the last N samples with the first N samples using one of three fade curves:

- **equal_power** (default): `fadeIn = sin(t × π/2)`, `fadeOut = cos(t × π/2)`. Maintains constant perceived loudness across the crossfade — perceptually the most natural and recommended for most audio.
- **linear**: `fadeIn = t`, `fadeOut = 1 − t`. Simple linear interpolation. Can produce a slight dip in perceived volume at the midpoint.
- **logarithmic**: `fadeIn = log₁₀(1 + 9t)`, `fadeOut = log₁₀(1 + 9(1−t))`. Approximates a decibel-scale curve, producing a slower initial fade and faster ramp near the ends.

Where `t = i / N` ranges from 0 to 1:

```
output[i] = start[i] * fadeIn(t) + end[i] * fadeOut(t)
```

The blended result overwrites the tail of the audio. When `-trimstart` is enabled, the first N samples (the fade-in portion) are removed so the loop region starts at the crossfaded boundary.

## Installation

```sh
cd tools/cmd/wav-crossfade-conv
go build -o ../../../build/tools/wav-crossfade-conv
```

Or via the bootstrap script:

```sh
./tools/script/bootstrap.sh
```

## Usage

```sh
wav-crossfade-conv [options]
```

### Options

| Option | Type | Description |
|--------|------|-------------|
| `-i <path>` | string | Input WAV file path (required) |
| `-o <path>` | string | Output WAV file path (required) |
| `-samples <n>` | int | Crossfade length in samples. Mutually exclusive with `-seconds` |
| `-seconds <n>` | float | Crossfade length in seconds. Converted to samples using the file's sample rate |
| `-trimstart` | bool | Remove the first N fade samples from the output, shortening the file |
| `-alignsamples <n>` | int | Align the loop region's sample count to a multiple of this value by trimming from the end |
| `-fade <method>` | string | Crossfade curve: `linear`, `equal_power` (default), or `logarithmic` |

### Examples

Apply a 2-second crossfade to an ambient sound:

```sh
./build/tools/wav-crossfade-conv \
    -i assets/sounds/ambient_raw.wav \
    -o assets/sounds/ambient_loop.wav \
    -seconds 2.0
```

Apply a crossfade with loop-start trimming (useful for seamless one-shot-to-loop transitions):

```sh
./build/tools/wav-crossfade-conv \
    -i assets/sounds/ambient_raw.wav \
    -o assets/sounds/ambient_loop.wav \
    -seconds 2.0 \
    -trimstart
```

Apply a crossfade with sample alignment to 4096-sample boundaries:

```sh
./build/tools/wav-crossfade-conv \
    -i assets/sounds/ambient_raw.wav \
    -o assets/sounds/ambient_loop.wav \
    -seconds 2.0 \
    -trimstart \
    -alignsamples 4096
```

## Typical Pipeline

This tool is used in the asset build pipeline alongside `ffmpeg`:

```sh
# 1. Convert source audio to PCM WAV (if needed)
ffmpeg -i source.ogg -ar 44100 -ac 2 -sample_fmt s16 source.wav

# 2. Apply crossfade for seamless looping
wav-crossfade-conv -i source.wav -o loop.wav -seconds 2.0 -trimstart

# 3. Convert to shipping format (e.g. Ogg Vorbis)
ffmpeg -i loop.wav -codec:a libvorbis -qscale:a 4 loop.ogg
```
