# archive-inspect

A diagnostic tool that reads archive index files (`.idx`, `.idx-debug`) produced by `archive-content` and prints their contents in a human-readable table or machine-readable JSON format.

Use this tool to verify the contents of an archive after building, to debug missing or misplaced assets, or to audit file sizes and offsets.

## Installation

```sh
cd tools/cmd/archive-inspect
go build -o ../../../build/tools/archive-inspect
```

## Usage

```sh
archive-inspect [options]
```

### Options

| Option | Description |
|--------|-------------|
| `-idx <path>` | Input index file (`.idx`). **Required** |
| `-pak <path>` | Input binary archive file (`.pak`). Used only for display in output |
| `-debug <path>` | Input debug index file (`.idx-debug`). Provides human-readable file paths |
| `-o <path>` | Output file path. If omitted, prints to stdout |
| `-json` | Output in JSON format instead of the default human-readable table |

### Examples

Inspect an archive with debug info (human-readable):

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/desktop/content.idx \
    -pak ./build/quickstart/shipping/desktop/content.pak \
    -debug ./build/quickstart/archivebuild/desktop/content.idx-debug
```

Inspect an archive in JSON format:

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/desktop/content.idx \
    -debug ./build/quickstart/archivebuild/desktop/content.idx-debug \
    -json
```

Inspect a shipping index without debug info:

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/desktop/content.idx
```

Save output to a file:

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/desktop/content.idx \
    -debug ./build/quickstart/archivebuild/desktop/content.idx-debug \
    -o archive-listing.txt
```
