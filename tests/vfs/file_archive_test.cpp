// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash64.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_archive.h"
#include "pomdog/vfs/file_archive_compressed.h"
#include "pomdog/vfs/file_system.h"
#include "pomdog/vfs/mount_volume.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/archive_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::i64;
using pomdog::u32;
using pomdog::u8;
using pomdog::vfs::ArchiveIOMethod;
using pomdog::vfs::File;
using pomdog::vfs::FileMode;
using pomdog::vfs::SeekOrigin;
using pomdog::vfs::detail::ArchiveDictionary;
using pomdog::vfs::detail::ArchiveEntryReader;
using pomdog::vfs::detail::CompressedArchiveFile;

namespace {

/// Returns the text that zstdFrame holds.
[[nodiscard]] std::string
makeEntryText()
{
    std::string text;
    for (int i = 0; i < 64; ++i) {
        text += pomdog::format("Pomdog archive entry {}\n", i);
    }
    return text;
}

/// A zstd frame of makeEntryText(), written by archive-content.
constexpr std::array<u8, 130> zstdFrame = {{
    // clang-format off
    0x28, 0xb5, 0x2f, 0xfd, 0x64, 0xf6, 0x04, 0xa5, 0x03, 0x00, 0x72, 0x05,
    0x11, 0x17, 0xa0, 0x29, 0x19, 0xa8, 0x0d, 0x40, 0x7a, 0x46, 0x52, 0x28,
    0x39, 0x80, 0xc7, 0xde, 0xfb, 0xa7, 0x94, 0x52, 0x6e, 0xfc, 0xa0, 0x15,
    0x0a, 0xa5, 0x55, 0x55, 0xff, 0xff, 0xff, 0xbf, 0x6d, 0xdb, 0xb6, 0xdd,
    0xb6, 0x6d, 0xdb, 0x92, 0x24, 0x49, 0x92, 0xdb, 0xb6, 0x6d, 0x4b, 0x90,
    0xa3, 0xee, 0x72, 0xc5, 0x30, 0x8b, 0x94, 0x40, 0x1c, 0x73, 0x14, 0x86,
    0x82, 0x20, 0x05, 0x0c, 0x01, 0x0d, 0x0e, 0x08, 0x10, 0x3f, 0xa8, 0x21,
    0xfc, 0xfa, 0xbf, 0x01, 0xa0, 0xe3, 0xd2, 0x01, 0x11, 0xfc, 0xaf, 0xa0,
    0xf8, 0x03, 0xf5, 0xad, 0x2a, 0x15, 0x73, 0xc9, 0x13, 0x07, 0x7c, 0xef,
    0xba, 0xd1, 0x1e, 0xbb, 0xf7, 0x1a, 0x58, 0xbd, 0xd2, 0xe5, 0x9b, 0x6a,
    0xd4, 0xbf, 0xc4, 0x0a, 0xfc, 0x2a, 0x0b, 0x96, 0x51, 0x3d,
    // clang-format on
}};

/// A zstd dictionary with ID 7, trained by the zstd command-line tool on small JSON files.
constexpr std::array<u8, 256> zstdDictionary = {{
    // clang-format off
    0x37, 0xa4, 0x30, 0xec, 0x07, 0x00, 0x00, 0x00, 0x15, 0x10, 0x10, 0xbd,
    0x74, 0xe0, 0xff, 0xff, 0xb5, 0xf4, 0xff, 0x5b, 0xb7, 0xdc, 0x52, 0xca,
    0x24, 0xb3, 0xf3, 0xd2, 0xc9, 0x0c, 0x83, 0x52, 0x18, 0x0c, 0x06, 0x34,
    0x83, 0xf3, 0x33, 0x33, 0xb3, 0x6d, 0x03, 0xf4, 0x00, 0xc1, 0x60, 0x60,
    0x60, 0x60, 0x60, 0x60, 0x60, 0xe0, 0xc1, 0x60, 0xe0, 0x49, 0x28, 0x50,
    0x63, 0x30, 0x18, 0x18, 0x28, 0x83, 0xc1, 0x60, 0x30, 0x18, 0xf8, 0x14,
    0xc3, 0x8c, 0x61, 0xc6, 0x30, 0x63, 0xcc, 0xcc, 0xcc, 0xcc, 0x6c, 0xdb,
    0x64, 0x07, 0x95, 0x53, 0xc2, 0xb0, 0xb0, 0x58, 0x0c, 0x82, 0x20, 0x08,
    0x82, 0x20, 0x08, 0x42, 0x08, 0x21, 0x84, 0x10, 0x42, 0x08, 0x11, 0x11,
    0x91, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x20, 0x22, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x73,
    0x2f, 0x73, 0x70, 0x72, 0x69, 0x74, 0x65, 0x5f, 0x31, 0x34, 0x2e, 0x70,
    0x6e, 0x67, 0x22, 0x2c, 0x20, 0x22, 0x73, 0x69, 0x7a, 0x65, 0x22, 0x3a,
    0x20, 0x5b, 0x31, 0x31, 0x32, 0x2c, 0x20, 0x35, 0x36, 0x5d, 0x2c, 0x20,
    0x22, 0x70, 0x69, 0x76, 0x20, 0x22, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72,
    0x65, 0x73, 0x2f, 0x73, 0x70, 0x72, 0x69, 0x74, 0x65, 0x5f, 0x31, 0x36,
    0x2e, 0x70, 0x6e, 0x67, 0x22, 0x2c, 0x20, 0x22, 0x73, 0x69, 0x7a, 0x65,
    0x22, 0x3a, 0x20, 0x5b, 0x31, 0x32, 0x38, 0x2c, 0x20, 0x36, 0x34, 0x5d,
    0x2c, 0x20, 0x22, 0x70, 0x69, 0x76, 0x30, 0x2e, 0x35, 0x2c, 0x20, 0x30,
    0x2e, 0x35, 0x5d, 0x2c, 0x20, 0x22, 0x66, 0x72, 0x61, 0x6d, 0x65, 0x73,
    0x22, 0x3a, 0x20, 0x32, 0x7d, 0x0a, 0x7b, 0x22, 0x6e, 0x61, 0x6d, 0x65,
    0x22, 0x3a, 0x20, 0x22,
    // clang-format on
}};

/// The text that zstdDictionaryFrame holds.
constexpr std::string_view dictionaryEntryText =
    R"({"name": "sprite_99", "texture": "textures/sprite_99.png", "size": [64, 32], "pivot": [0.5, 0.5], "frames": 3})"
    "\n";

/// A zstd frame of dictionaryEntryText, compressed with zstdDictionary by the
/// zstd command-line tool. The frame records the dictionary ID.
constexpr std::array<u8, 56> zstdDictionaryFrame = {{
    // clang-format off
    0x28, 0xb5, 0x2f, 0xfd, 0x25, 0x07, 0x6f, 0x55, 0x01, 0x00, 0x90, 0x7b,
    0x39, 0x39, 0x22, 0x2c, 0x39, 0x39, 0x36, 0x34, 0x2c, 0x20, 0x33, 0x32,
    0x6f, 0x74, 0x33, 0x7d, 0x0a, 0x09, 0xc0, 0x7c, 0x78, 0x6c, 0x6c, 0xb6,
    0x71, 0x06, 0x40, 0xba, 0xc3, 0xfc, 0x0e, 0x10, 0xb0, 0x58, 0xb2, 0x70,
    0xde, 0xa2, 0x0c, 0x0e, 0x22, 0x8b, 0xcd, 0xe9,
    // clang-format on
}};

/// Returns `bytes` as text.
[[nodiscard]] std::string
toString(std::span<const u8> bytes)
{
    return std::string(bytes.begin(), bytes.end());
}

/// Returns bytes that do not compress, larger than the chunks that the file
/// stream reader reads.
[[nodiscard]] std::vector<u8>
makeLargeContent()
{
    std::vector<u8> content(200 * 1024);
    u32 state = 20160723;
    for (auto& value : content) {
        state = state * 1664525 + 1013904223;
        value = static_cast<u8>(state >> 24);
    }
    return content;
}

/// Returns a zstd frame that stores `content` in raw blocks. It needs no
/// compressor, so tests can build frames larger than embedded fixtures.
[[nodiscard]] std::vector<u8>
makeRawBlockFrame(std::span<const u8> content)
{
    std::vector<u8> frame = {0x28, 0xb5, 0x2f, 0xfd};

    // NOTE: A single-segment frame with a 4-byte content size, no checksum, and no dictionary.
    frame.push_back(0xa0);
    const auto contentSize = static_cast<u32>(content.size());
    for (u32 shift = 0; shift < 32; shift += 8) {
        frame.push_back(static_cast<u8>(contentSize >> shift));
    }

    constexpr std::size_t maxBlockSize = 128 * 1024;
    std::size_t offset = 0;
    while (offset < content.size()) {
        const auto blockSize = std::min(maxBlockSize, content.size() - offset);
        const bool lastBlock = (offset + blockSize == content.size());

        // NOTE: Block header: Last_Block in bit 0, Block_Type 0 (raw) in bits 1-2, and Block_Size above.
        const auto header = (static_cast<u32>(blockSize) << 3) | (lastBlock ? 1u : 0u);
        for (u32 shift = 0; shift < 24; shift += 8) {
            frame.push_back(static_cast<u8>(header >> shift));
        }

        const auto block = content.subspan(offset, blockSize);
        frame.insert(frame.end(), block.begin(), block.end());
        offset += blockSize;
    }
    return frame;
}

/// ChunkedReader supplies stored bytes in chunks of at most `chunkSize` bytes.
class ChunkedReader final : public ArchiveEntryReader {
private:
    std::vector<u8> bytes_;
    std::size_t chunkSize_ = 0;

public:
    ChunkedReader(std::span<const u8> bytes, std::size_t chunkSize)
        : bytes_(bytes.begin(), bytes.end())
        , chunkSize_(chunkSize)
    {
    }

    ChunkedReader(const ChunkedReader&) = delete;
    ChunkedReader& operator=(const ChunkedReader&) = delete;
    ChunkedReader(ChunkedReader&&) = delete;
    ChunkedReader& operator=(ChunkedReader&&) = delete;

    ~ChunkedReader() noexcept override = default;

    [[nodiscard]] std::tuple<std::span<const u8>, std::unique_ptr<pomdog::Error>>
    read(std::size_t offset) noexcept override
    {
        if (offset >= bytes_.size()) {
            return {std::span<const u8>{}, nullptr};
        }
        const auto size = std::min(chunkSize_, bytes_.size() - offset);
        return {std::span<const u8>{bytes_}.subspan(offset, size), nullptr};
    }
};

[[nodiscard]] std::shared_ptr<File>
makeZstdFile(std::span<const u8> stored, std::size_t chunkSize, std::size_t uncompressedSize)
{
    return std::make_shared<CompressedArchiveFile>(
        std::make_unique<ChunkedReader>(stored, chunkSize),
        pomdogschemas::ArchiveCompression_Zstd,
        nullptr,
        uncompressedSize);
}

[[nodiscard]] std::shared_ptr<File>
makeZstdDictionaryFile(std::span<const u8> stored, std::shared_ptr<const ArchiveDictionary> dictionary)
{
    return std::make_shared<CompressedArchiveFile>(
        std::make_unique<ChunkedReader>(stored, 16),
        pomdogschemas::ArchiveCompression_ZstdDictionary,
        std::move(dictionary),
        dictionaryEntryText.size());
}

/// Reads the whole file in one call and returns its content as text.
[[nodiscard]] std::tuple<std::string, std::unique_ptr<pomdog::Error>>
readAll(File& file)
{
    auto [size, sizeErr] = file.getSize();
    if (sizeErr != nullptr) {
        return {std::string{}, std::move(sizeErr)};
    }
    std::vector<u8> buffer(size);
    auto [bytesRead, readErr] = file.read(buffer);
    if (readErr != nullptr) {
        return {std::string{}, std::move(readErr)};
    }
    return {toString(std::span<const u8>{buffer}.first(bytesRead)), nullptr};
}

/// Writes `bytes` to `path`, replacing the file.
void writeFile(const std::filesystem::path& path, std::span<const u8> bytes)
{
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    REQUIRE(stream.is_open());
    stream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    REQUIRE(stream.good());
}

constexpr std::string_view rawText = "hello archive";

/// TestArchive is a .idx and .pak pair with uncompressed, zstd, and malformed entries.
struct TestArchive final {
    std::filesystem::path idxPath;
    std::filesystem::path pakPath;
};

/// Writes a test archive whose index holds `dictionary` for the entries
/// compressed with a dictionary.
[[nodiscard]] TestArchive
writeTestArchive(std::span<const u8> dictionary)
{
    auto [tempDir, tempErr] = pomdog::platformfs::getTempDirectoryPath();
    REQUIRE(tempErr == nullptr);

    TestArchive result;
    result.idxPath = std::filesystem::path{tempDir} / "pomdog_file_archive_test.idx";
    result.pakPath = std::filesystem::path{tempDir} / "pomdog_file_archive_test.pak";

    // NOTE: The .pak file holds the raw text, the zstd frame, the zstd frame
    // compressed with the dictionary, and the large raw-block frame.
    const auto largeContent = makeLargeContent();
    const auto largeFrame = makeRawBlockFrame(largeContent);
    std::vector<u8> pak(rawText.begin(), rawText.end());
    pak.insert(pak.end(), zstdFrame.begin(), zstdFrame.end());
    pak.insert(pak.end(), zstdDictionaryFrame.begin(), zstdDictionaryFrame.end());
    pak.insert(pak.end(), largeFrame.begin(), largeFrame.end());
    writeFile(result.pakPath, pak);

    const auto rawSize = static_cast<u32>(rawText.size());
    const auto frameSize = static_cast<u32>(zstdFrame.size());
    const auto textSize = static_cast<u32>(makeEntryText().size());
    const auto dictionaryFrameOffset = rawSize + frameSize;
    const auto dictionaryFrameSize = static_cast<u32>(zstdDictionaryFrame.size());
    const auto dictionaryTextSize = static_cast<u32>(dictionaryEntryText.size());
    const auto largeOffset = dictionaryFrameOffset + dictionaryFrameSize;

    pomdogschemas::ArchiveT archive;

    auto archiveDictionary = std::make_unique<pomdogschemas::ArchiveDictionaryT>();
    archiveDictionary->data.assign(dictionary.begin(), dictionary.end());
    archive.dictionaries.push_back(std::move(archiveDictionary));

    const auto addKey = [&](std::string_view path, u32 index) {
        archive.keys.emplace_back(pomdog::computeStringHash64(path), index);
    };
    const auto addEntry = [&](std::string_view path, u32 offset, u32 uncompressedSize, u32 compressedSize, pomdogschemas::ArchiveCompression compression, u8 dictionaryIndex = 0) {
        addKey(path, static_cast<u32>(archive.entries.size()));
        archive.entries.emplace_back(offset, uncompressedSize, compressedSize, compression, dictionaryIndex);
    };
    addEntry("raw.txt", 0, rawSize, rawSize, pomdogschemas::ArchiveCompression_None);
    addEntry("entry.txt", rawSize, textSize, frameSize, pomdogschemas::ArchiveCompression_Zstd);
    addEntry("dictionary.json", dictionaryFrameOffset, dictionaryTextSize, dictionaryFrameSize, pomdogschemas::ArchiveCompression_ZstdDictionary);
    addEntry("large.bin", largeOffset, static_cast<u32>(largeContent.size()), static_cast<u32>(largeFrame.size()), pomdogschemas::ArchiveCompression_Zstd);
    addEntry("mismatched_sizes.txt", 0, rawSize, rawSize - 1, pomdogschemas::ArchiveCompression_None);
    addEntry("unknown_compression.txt", 0, rawSize, rawSize, static_cast<pomdogschemas::ArchiveCompression>(7));
    addEntry("outside_pak.txt", largeOffset, static_cast<u32>(largeContent.size()), static_cast<u32>(largeFrame.size()) + 1, pomdogschemas::ArchiveCompression_Zstd);
    addEntry("missing_dictionary.json", dictionaryFrameOffset, dictionaryTextSize, dictionaryFrameSize, pomdogschemas::ArchiveCompression_ZstdDictionary, 1);

    // NOTE: Keys of files with the same content share the entries of the first files.
    addKey("entry_copy.txt", 1);
    addKey("dictionary_copy.json", 2);

    // NOTE: LookupByKey() needs the keys sorted, and Archive::Pack() writes
    // the keys in the order of the vector.
    std::ranges::sort(archive.keys, {}, [](const auto& key) { return key.key(); });

    flatbuffers::FlatBufferBuilder builder;
    pomdogschemas::FinishArchiveBuffer(builder, pomdogschemas::Archive::Pack(builder, &archive));

    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto indexBytes = std::span<const u8>{builder.GetBufferPointer(), builder.GetSize()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
    writeFile(result.idxPath, indexBytes);

    return result;
}

void checkArchiveVolume(ArchiveIOMethod ioMethod)
{
    const auto archive = writeTestArchive(zstdDictionary);
    {
        auto [volume, openErr] = pomdog::vfs::openArchiveFile(archive.idxPath.string(), archive.pakPath.string(), ioMethod);
        REQUIRE(openErr == nullptr);
        REQUIRE(volume != nullptr);

        FileMode mode;
        mode.read = true;

        // NOTE: An uncompressed entry.
        {
            auto [file, err] = volume->open("raw.txt", mode);
            REQUIRE(err == nullptr);
            auto [text, readErr] = readAll(*file);
            REQUIRE(readErr == nullptr);
            REQUIRE(text == rawText);
        }

        // NOTE: A zstd entry read whole.
        {
            auto [file, err] = volume->open("entry.txt", mode);
            REQUIRE(err == nullptr);
            auto [text, readErr] = readAll(*file);
            REQUIRE(readErr == nullptr);
            REQUIRE(text == makeEntryText());

            auto [info, statErr] = volume->stat("entry.txt");
            REQUIRE(statErr == nullptr);
            REQUIRE(info.size == makeEntryText().size());
        }

        // NOTE: A zstd entry read in parts, as the audio loader reads its header.
        {
            auto [file, err] = volume->open("entry.txt", mode);
            REQUIRE(err == nullptr);
            std::array<u8, 12> header = {};
            auto [bytesRead, readErr] = file->read(header);
            REQUIRE(readErr == nullptr);
            REQUIRE(bytesRead == header.size());
            const auto headerText = toString(header);
            REQUIRE(headerText == "Pomdog archi");

            auto [pos, seekErr] = file->seek(0, SeekOrigin::Begin);
            REQUIRE(seekErr == nullptr);
            REQUIRE(pos == 0);
            auto [text, readAllErr] = readAll(*file);
            REQUIRE(readAllErr == nullptr);
            REQUIRE(text == makeEntryText());
        }

        // NOTE: A zstd entry whose stored bytes span several reads of the file stream.
        {
            auto [file, err] = volume->open("large.bin", mode);
            REQUIRE(err == nullptr);
            const auto expected = makeLargeContent();
            std::vector<u8> content(expected.size());
            auto [bytesRead, readErr] = file->read(content);
            REQUIRE(readErr == nullptr);
            REQUIRE(bytesRead == expected.size());
            REQUIRE(content == expected);
        }

        // NOTE: Entries compressed with a dictionary, and keys that share entries.
        for (const auto* path : {"dictionary.json", "dictionary_copy.json"}) {
            auto [file, err] = volume->open(path, mode);
            REQUIRE(err == nullptr);
            auto [text, readErr] = readAll(*file);
            REQUIRE(readErr == nullptr);
            REQUIRE(text == dictionaryEntryText);
        }
        {
            auto [file, err] = volume->open("entry_copy.txt", mode);
            REQUIRE(err == nullptr);
            auto [text, readErr] = readAll(*file);
            REQUIRE(readErr == nullptr);
            REQUIRE(text == makeEntryText());
        }

        // NOTE: Malformed and missing entries fail to open and to stat.
        for (const auto* path : {"mismatched_sizes.txt", "unknown_compression.txt", "outside_pak.txt", "missing_dictionary.json", "missing.txt"}) {
            auto [file, err] = volume->open(path, mode);
            REQUIRE(err != nullptr);
            REQUIRE(file == nullptr);
            auto [info, statErr] = volume->stat(path);
            REQUIRE(statErr != nullptr);
        }

        // NOTE: A file keeps its dictionary after the volume is released below.
        auto [dictionaryFile, dictionaryFileErr] = volume->open("dictionary.json", mode);
        REQUIRE(dictionaryFileErr == nullptr);

        // NOTE: The VFS reads a zstd entry through a mounted archive.
        {
            auto [ctx, ctxErr] = pomdog::vfs::create();
            REQUIRE(ctxErr == nullptr);
            pomdog::vfs::MountOptions options;
            options.readOnly = true;
            options.hashKeyLookup = true;
            REQUIRE(pomdog::vfs::mount(ctx, "/assets", std::move(volume), options) == nullptr);

            auto [file, err] = pomdog::vfs::open(ctx, "/assets/entry.txt");
            REQUIRE(err == nullptr);
            auto [text, readErr] = readAll(*file);
            REQUIRE(readErr == nullptr);
            REQUIRE(text == makeEntryText());
        }
        {
            auto [text, readErr] = readAll(*dictionaryFile);
            REQUIRE(readErr == nullptr);
            REQUIRE(text == dictionaryEntryText);
        }
    }

    // NOTE: Remove the files after the volume closes them.
    std::error_code removeErr;
    std::filesystem::remove(archive.idxPath, removeErr);
    std::filesystem::remove(archive.pakPath, removeErr);
}

} // namespace

TEST_CASE("VFS.CompressedArchiveFile")
{
    const auto text = makeEntryText();

    SUBCASE("reads a whole zstd entry from chunks of any size")
    {
        for (const std::size_t chunkSize : {std::size_t{1}, std::size_t{7}, zstdFrame.size()}) {
            auto file = makeZstdFile(zstdFrame, chunkSize, text.size());
            auto [content, err] = readAll(*file);
            REQUIRE(err == nullptr);
            REQUIRE(content == text);
        }
    }
    SUBCASE("reads a large zstd entry in parts from chunks")
    {
        const auto expected = makeLargeContent();
        const auto frame = makeRawBlockFrame(expected);
        auto file = makeZstdFile(frame, 1000, expected.size());

        std::vector<u8> content(expected.size());
        std::size_t position = 0;
        while (position < content.size()) {
            const auto size = std::min(std::size_t{4096}, content.size() - position);
            auto [bytesRead, err] = file->read(std::span<u8>{content}.subspan(position, size));
            REQUIRE(err == nullptr);
            REQUIRE(bytesRead == size);
            position += bytesRead;
        }
        REQUIRE(content == expected);
    }
    SUBCASE("reads a zstd entry in parts")
    {
        auto file = makeZstdFile(zstdFrame, 16, text.size());
        std::array<u8, 21> first = {};
        auto [firstRead, firstErr] = file->read(first);
        REQUIRE(firstErr == nullptr);
        REQUIRE(firstRead == first.size());
        const auto firstText = toString(first);
        REQUIRE(firstText == "Pomdog archive entry ");

        auto [pos, seekErr] = file->seek(-3, SeekOrigin::End);
        REQUIRE(seekErr == nullptr);
        REQUIRE(pos == static_cast<i64>(text.size() - 3));

        std::array<u8, 8> last = {};
        auto [lastRead, lastErr] = file->read(last);
        REQUIRE(lastErr == nullptr);
        REQUIRE(lastRead == 3);
        const auto lastText = toString(std::span<const u8>{last}.first(3));
        REQUIRE(lastText == "63\n");

        auto [rewound, rewindErr] = file->seek(0, SeekOrigin::Begin);
        REQUIRE(rewindErr == nullptr);
        REQUIRE(rewound == 0);
        auto [content, err] = readAll(*file);
        REQUIRE(err == nullptr);
        REQUIRE(content == text);
    }
    SUBCASE("reports the uncompressed size")
    {
        auto file = makeZstdFile(zstdFrame, 64, text.size());
        auto [info, statErr] = file->stat();
        REQUIRE(statErr == nullptr);
        REQUIRE(info.size == text.size());
        auto [size, sizeErr] = file->getSize();
        REQUIRE(sizeErr == nullptr);
        REQUIRE(size == text.size());
    }
    SUBCASE("rejects a corrupted frame")
    {
        auto corrupted = zstdFrame;
        corrupted.at(corrupted.size() / 2) ^= u8{0x5a};
        auto file = makeZstdFile(corrupted, 64, text.size());
        auto [content, err] = readAll(*file);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects a size that differs from the frame")
    {
        for (const auto size : {text.size() - 1, text.size() + 1}) {
            auto file = makeZstdFile(zstdFrame, 64, size);
            auto [content, err] = readAll(*file);
            REQUIRE(err != nullptr);
        }
    }
    SUBCASE("rejects a truncated frame")
    {
        auto file = makeZstdFile(std::span<const u8>{zstdFrame}.first(zstdFrame.size() - 4), 64, text.size());
        auto [content, err] = readAll(*file);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects data after the frame")
    {
        std::vector<u8> stored(zstdFrame.begin(), zstdFrame.end());
        stored.push_back(0);
        auto file = makeZstdFile(stored, 64, text.size());
        auto [content, err] = readAll(*file);
        REQUIRE(err != nullptr);
    }
    SUBCASE("fails to read after close")
    {
        auto file = makeZstdFile(zstdFrame, 64, text.size());
        REQUIRE(file->close() == nullptr);
        std::array<u8, 4> buffer = {};
        auto [bytesRead, err] = file->read(buffer);
        REQUIRE(err != nullptr);
        REQUIRE(bytesRead == 0);
    }
    SUBCASE("reads a zstd entry compressed with a dictionary")
    {
        auto [dictionary, dictionaryErr] = pomdog::vfs::detail::createArchiveDictionary(zstdDictionary);
        REQUIRE(dictionaryErr == nullptr);
        REQUIRE(dictionary != nullptr);

        // NOTE: Files share the dictionary, and read whole or in parts.
        auto file = makeZstdDictionaryFile(zstdDictionaryFrame, dictionary);
        auto [content, err] = readAll(*file);
        REQUIRE(err == nullptr);
        REQUIRE(content == dictionaryEntryText);

        auto partFile = makeZstdDictionaryFile(zstdDictionaryFrame, dictionary);
        std::array<u8, 9> first = {};
        auto [firstRead, firstErr] = partFile->read(first);
        REQUIRE(firstErr == nullptr);
        REQUIRE(firstRead == first.size());
        const auto firstText = toString(first);
        REQUIRE(firstText == R"({"name": )");
    }
    SUBCASE("rejects a dictionary with another ID")
    {
        auto otherDictionary = zstdDictionary;
        otherDictionary.at(4) = u8{8};
        auto [dictionary, dictionaryErr] = pomdog::vfs::detail::createArchiveDictionary(otherDictionary);
        REQUIRE(dictionaryErr == nullptr);

        auto file = makeZstdDictionaryFile(zstdDictionaryFrame, dictionary);
        auto [content, err] = readAll(*file);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects an entry without its dictionary")
    {
        auto file = makeZstdDictionaryFile(zstdDictionaryFrame, nullptr);
        auto [content, err] = readAll(*file);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects bytes that are not a zstd dictionary")
    {
        for (const auto bytes : {std::span<const u8>{zstdFrame}, std::span<const u8>{zstdDictionary}.first(8)}) {
            auto [dictionary, err] = pomdog::vfs::detail::createArchiveDictionary(bytes);
            REQUIRE(err != nullptr);
            REQUIRE(dictionary == nullptr);
        }
    }
}

TEST_CASE("VFS.ArchiveVolume")
{
    SUBCASE("file stream")
    {
        checkArchiveVolume(ArchiveIOMethod::FileStream);
    }
    SUBCASE("memory-mapped I/O where supported")
    {
        checkArchiveVolume(ArchiveIOMethod::PreferMmap);
    }
    SUBCASE("rejects an index with an invalid dictionary")
    {
        const auto archive = writeTestArchive(zstdFrame);
        for (const auto ioMethod : {ArchiveIOMethod::FileStream, ArchiveIOMethod::PreferMmap}) {
            auto [volume, openErr] = pomdog::vfs::openArchiveFile(archive.idxPath.string(), archive.pakPath.string(), ioMethod);
            REQUIRE(openErr != nullptr);
            REQUIRE(volume == nullptr);
        }

        std::error_code removeErr;
        std::filesystem::remove(archive.idxPath, removeErr);
        std::filesystem::remove(archive.pakPath, removeErr);
    }
}
