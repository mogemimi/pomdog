// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/file_archive_compressed.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/memory/memcpy_span.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file_archive_entry.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/archive_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

// NOTE: ZSTD_d_stableOutBuffer and ZSTD_createDDict_advanced() belong to the
// experimental API, which zstd exposes only to programs that link it
// statically, as Pomdog does.
#define ZSTD_STATIC_LINKING_ONLY

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <zstd.h>
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs::detail {

/// ArchiveDictionary owns a zstd decoding dictionary, which decoders can use
/// from several threads at once.
class ArchiveDictionary final {
private:
    ZSTD_DDict* dictionary_ = nullptr;

public:
    explicit ArchiveDictionary(ZSTD_DDict* dictionary) noexcept
        : dictionary_(dictionary)
    {
    }

    ArchiveDictionary(const ArchiveDictionary&) = delete;
    ArchiveDictionary& operator=(const ArchiveDictionary&) = delete;
    ArchiveDictionary(ArchiveDictionary&&) = delete;
    ArchiveDictionary& operator=(ArchiveDictionary&&) = delete;

    ~ArchiveDictionary() noexcept
    {
        ZSTD_freeDDict(dictionary_);
    }

    [[nodiscard]] const ZSTD_DDict*
    get() const noexcept
    {
        return dictionary_;
    }
};

namespace {

/// ZstdContextDeleter frees a zstd decompression context.
struct ZstdContextDeleter final {
    void operator()(ZSTD_DCtx* context) const noexcept
    {
        ZSTD_freeDCtx(context);
    }
};

/// Decodes the single zstd frame that `reader` supplies into `dst`, whose size
/// is the content size of the frame. The frame decodes with `dictionary` unless
/// it is nullptr.
[[nodiscard]] std::unique_ptr<Error>
decodeZstd(ArchiveEntryReader& reader, const ArchiveDictionary* dictionary, std::span<u8> dst) noexcept
{
    std::unique_ptr<ZSTD_DCtx, ZstdContextDeleter> context{ZSTD_createDCtx()};
    if (context == nullptr) {
        return errors::make("ZSTD_createDCtx() failed");
    }

    if (dictionary != nullptr) {
        // NOTE: The context refers to the dictionary without copying it.
        if (const auto result = ZSTD_DCtx_refDDict(context.get(), dictionary->get()); ZSTD_isError(result)) {
            return errors::make(pomdog::format("ZSTD_DCtx_refDDict() failed: {}", ZSTD_getErrorName(result)));
        }
    }

    // NOTE: With a stable output buffer, the decoder writes directly into `dst`
    // and reads the history from there. It allocates no window buffer, and the
    // compressed input can arrive in chunks of any size.
    if (const auto result = ZSTD_DCtx_setParameter(context.get(), ZSTD_d_stableOutBuffer, 1); ZSTD_isError(result)) {
        return errors::make(pomdog::format("ZSTD_DCtx_setParameter() failed: {}", ZSTD_getErrorName(result)));
    }

    ZSTD_outBuffer output = {
        .dst = dst.data(),
        .size = dst.size(),
        .pos = 0,
    };
    std::size_t offset = 0;
    bool frameEnded = false;

    for (;;) {
        auto [chunk, readErr] = reader.read(offset);
        if (readErr != nullptr) {
            return errors::wrap(std::move(readErr), "failed to read compressed archive entry");
        }
        if (chunk.empty()) {
            break;
        }
        if (frameEnded) {
            return errors::make("unexpected data after the zstd frame");
        }
        offset += chunk.size();

        ZSTD_inBuffer input = {
            .src = chunk.data(),
            .size = chunk.size(),
            .pos = 0,
        };
        while (input.pos < input.size) {
            const auto inputPosition = input.pos;
            const auto outputPosition = output.pos;

            const auto result = ZSTD_decompressStream(context.get(), &output, &input);
            if (ZSTD_isError(result)) {
                return errors::make(pomdog::format("failed to decode zstd frame: {}", ZSTD_getErrorName(result)));
            }
            if (result == 0) {
                frameEnded = true;
                if (input.pos < input.size) {
                    return errors::make("unexpected data after the zstd frame");
                }
                break;
            }
            if (input.pos == inputPosition && output.pos == outputPosition) {
                return errors::make("zstd frame does not fit in the archive entry size");
            }
        }
    }

    if (!frameEnded) {
        return errors::make("zstd frame is truncated");
    }
    if (output.pos != dst.size()) {
        return errors::make(pomdog::format(
            "zstd frame holds {} bytes, but the archive entry holds {} bytes",
            output.pos,
            dst.size()));
    }
    return nullptr;
}

} // namespace

std::tuple<std::shared_ptr<const ArchiveDictionary>, std::unique_ptr<Error>>
createArchiveDictionary(std::span<const u8> data) noexcept
{
    // NOTE: The dictionary keeps its own copy of `data`. It accepts only the
    // Zstandard dictionary format, which the archive tools write, and rejects
    // other bytes instead of using them as raw content.
    auto* dictionary = ZSTD_createDDict_advanced(
        data.data(),
        data.size(),
        ZSTD_dlm_byCopy,
        ZSTD_dct_fullDict,
        ZSTD_customMem{.customAlloc = nullptr, .customFree = nullptr, .opaque = nullptr});
    if (dictionary == nullptr) {
        return {nullptr, errors::make("invalid zstd dictionary")};
    }
    return {std::make_shared<const ArchiveDictionary>(dictionary), nullptr};
}

std::tuple<std::vector<std::shared_ptr<const ArchiveDictionary>>, std::unique_ptr<Error>>
createArchiveDictionaries(const pomdogschemas::Archive& archive) noexcept
{
    std::vector<std::shared_ptr<const ArchiveDictionary>> result;

    const auto* dictionaries = archive.dictionaries();
    if (dictionaries == nullptr) {
        return {std::move(result), nullptr};
    }

    result.reserve(dictionaries->size());
    for (flatbuffers::uoffset_t i = 0; i < dictionaries->size(); ++i) {
        const auto* dictionary = dictionaries->Get(i);
        if ((dictionary == nullptr) || (dictionary->data() == nullptr)) {
            return {std::vector<std::shared_ptr<const ArchiveDictionary>>{}, errors::make(pomdog::format("archive dictionary {} has no data", i))};
        }

        const auto* data = dictionary->data();
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        const auto bytes = std::span<const u8>{data->data(), data->size()};
        POMDOG_CLANG_UNSAFE_BUFFER_END

        auto [decodingDictionary, err] = createArchiveDictionary(bytes);
        if (err != nullptr) {
            return {std::vector<std::shared_ptr<const ArchiveDictionary>>{}, errors::wrap(std::move(err), pomdog::format("archive dictionary {}", i))};
        }
        result.push_back(std::move(decodingDictionary));
    }
    return {std::move(result), nullptr};
}

ArchiveEntryReader::ArchiveEntryReader() noexcept = default;

ArchiveEntryReader::~ArchiveEntryReader() noexcept = default;

CompressedArchiveFile::CompressedArchiveFile(
    std::unique_ptr<ArchiveEntryReader> reader,
    pomdogschemas::ArchiveCompression compression,
    std::shared_ptr<const ArchiveDictionary> dictionary,
    std::size_t uncompressedSize) noexcept
    : reader_(std::move(reader))
    , dictionary_(std::move(dictionary))
    , size_(uncompressedSize)
    , compression_(compression)
{
}

CompressedArchiveFile::~CompressedArchiveFile() noexcept = default;

std::tuple<std::size_t, std::unique_ptr<Error>>
CompressedArchiveFile::read(std::span<u8> buffer) noexcept
{
    if (closed_) {
        return {0, errors::make("file is closed")};
    }
    if (buffer.empty()) {
        // NOTE: Return before decoding, so that an empty read does not make the
        // file keep a decoded copy of the entry.
        return {0, nullptr};
    }

    if (size_ == 0 && !content_.isAllocated()) {
        if (auto err = decodeContent(); err != nullptr) {
            return {0, std::move(err)};
        }
    }
    if (position_ >= size_) {
        return {0, nullptr};
    }

    if (!content_.isAllocated()) {
        if ((position_ == 0) && (buffer.size() >= size_)) {
            // NOTE: Decode a read of the whole entry directly into the caller's buffer.
            if (auto err = decode(buffer.first(size_)); err != nullptr) {
                return {0, std::move(err)};
            }
            position_ = size_;
            return {size_, nullptr};
        }

        if (auto err = decodeContent(); err != nullptr) {
            return {0, std::move(err)};
        }
    }

    const auto toRead = std::min(buffer.size(), size_ - position_);
    memcpySpan(buffer.first(toRead), content_.bytes().subspan(position_, toRead));
    position_ += toRead;
    return {toRead, nullptr};
}

std::tuple<std::size_t, std::unique_ptr<Error>>
CompressedArchiveFile::write([[maybe_unused]] std::span<const u8> buffer) noexcept
{
    return {0, errors::make("archive files are read-only")};
}

std::tuple<i64, std::unique_ptr<Error>>
CompressedArchiveFile::seek(i64 offset, SeekOrigin origin) noexcept
{
    auto [newPos, err] = resolveArchiveSeek(position_, size_, offset, origin);
    if (err != nullptr) {
        return {0, std::move(err)};
    }

    position_ = static_cast<std::size_t>(newPos);
    return {newPos, nullptr};
}

std::unique_ptr<Error>
CompressedArchiveFile::close() noexcept
{
    reader_ = nullptr;
    content_.reset();
    closed_ = true;
    return nullptr;
}

std::tuple<FileInfo, std::unique_ptr<Error>>
CompressedArchiveFile::stat() noexcept
{
    FileInfo info = {};
    info.size = size_;
    info.isDirectory = false;
    return {std::move(info), nullptr};
}

std::tuple<std::size_t, std::unique_ptr<Error>>
CompressedArchiveFile::getSize() noexcept
{
    return {size_, nullptr};
}

std::tuple<std::span<const u8>, std::unique_ptr<Error>>
CompressedArchiveFile::view() noexcept
{
    if (closed_) {
        return {std::span<const u8>{}, errors::make("file is closed")};
    }
    if (!content_.isAllocated()) {
        if (auto err = decodeContent(); err != nullptr) {
            return {std::span<const u8>{}, std::move(err)};
        }
    }
    return {content_.bytes(), nullptr};
}

std::unique_ptr<Error>
CompressedArchiveFile::decodeContent() noexcept
{
    if (auto err = content_.allocate(size_); err != nullptr) {
        return err;
    }
    if (auto err = decode(content_.bytes()); err != nullptr) {
        content_.reset();
        return err;
    }

    // NOTE: The decoded content serves every later read, so release the stored bytes.
    reader_ = nullptr;
    return nullptr;
}

std::unique_ptr<Error>
CompressedArchiveFile::decode(std::span<u8> dst) noexcept
{
    if (reader_ == nullptr) {
        return errors::make("compressed archive entry has no stored bytes");
    }
    if (compression_ == pomdogschemas::ArchiveCompression_Zstd) {
        return decodeZstd(*reader_, nullptr, dst);
    }
    if (compression_ == pomdogschemas::ArchiveCompression_ZstdDictionary) {
        if (dictionary_ == nullptr) {
            return errors::make("zstd archive entry has no dictionary");
        }
        return decodeZstd(*reader_, dictionary_.get(), dst);
    }
    return errors::make(pomdog::format("unsupported archive compression {}", static_cast<u32>(compression_)));
}

} // namespace pomdog::vfs::detail
