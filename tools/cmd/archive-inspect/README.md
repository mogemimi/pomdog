# archive-inspect

A diagnostic tool that reads archive index files (`.idx`, `.idx-debug`) produced by `archive-content` and prints their contents in a human-readable table or machine-readable JSON format.

Use this tool to verify the contents of an archive after building, to debug missing or misplaced assets, or to audit file sizes, offsets, and compression ratios.

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
| `-pak <path>` | Input binary archive file (`.pak`). Checks that every entry lies inside the file and reads the frame header of each zstd entry |
| `-debug <path>` | Input debug index file (`.idx-debug`). Provides human-readable file paths, content hashes, and dictionary names |
| `-o <path>` | Output file path. If omitted, prints to stdout |
| `-json` | Output in JSON format instead of the default human-readable table |

### Output

The summary shows the total size of the original files and the bytes stored in the `.pak` file, counting bytes that files share once. The totals for each compression method count the stored bytes of every file. The summary also shows the bytes that shared data saves, and the dictionaries in the index with the number of files that use each and the bytes they save compared with storing the files as-is. Each entry lists its original size (`SIZE`), stored bytes (`STORED`), the stored size as a percentage of the original (`RATIO`), and its compression method with its dictionary, such as `zstd_dictionary(structured)`. The groups of files that share stored bytes follow the entries, one path per line.

The tool fails if an entry refers to a dictionary that the index does not have, or if a dictionary is not in the Zstandard dictionary format. With `-pak`, it also fails if an entry extends past the end of the `.pak` file, if a zstd frame lacks a content size that matches the index, or if a frame records the ID of another dictionary. It also reports the largest zstd window, which is the history a streaming decoder keeps for an entry.

With `-debug`, the tool also lists:

- Entries stored as-is although the archive settings or recipe group requested zstd, in a table with the size and ratio that the requested method and dictionary produced. Use it to tune the thresholds and dictionaries in the archive settings.
- Files with identical content that the archive stores in more than one place, using the content hashes in the debug index, such as the same file in a group with another compression method.
- Diagnostics for the dictionaries in the archive settings:

| Kind | Meaning |
|------|---------|
| `small_training_data` | The files that use the dictionary have less than 10 times its size of training data, which zstd recommends at least. zstd reads up to 128 KiB from each file. A smaller `max_size` may make the archive smaller |
| `dictionary_larger_than_savings` | The dictionary takes more bytes in the index than its files save compared with storing them as-is, so the archive is smaller without it |
| `unused_dictionary` | Every file that requested the dictionary is stored as-is, so the index does not store the dictionary |

### Examples

Inspect an archive with debug info (human-readable):

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/windows/content.idx \
    -pak ./build/quickstart/shipping/windows/content.pak \
    -debug ./build/quickstart/archivebuild/windows/content.idx-debug
```

Inspect an archive in JSON format:

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/windows/content.idx \
    -debug ./build/quickstart/archivebuild/windows/content.idx-debug \
    -json
```

Inspect a shipping index without debug info:

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/windows/content.idx
```

Save output to a file:

```sh
./build/tools/archive-inspect \
    -idx ./build/quickstart/shipping/windows/content.idx \
    -debug ./build/quickstart/archivebuild/windows/content.idx-debug \
    -o archive-listing.txt
```
