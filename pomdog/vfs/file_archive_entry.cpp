// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/vfs/file_archive_entry.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/not_found_error.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/archive_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs::detail {

std::tuple<i64, std::unique_ptr<Error>>
resolveArchiveSeek(std::size_t position, std::size_t size, i64 offset, SeekOrigin origin) noexcept
{
    if (position > size || static_cast<u64>(size) > static_cast<u64>(std::numeric_limits<i64>::max())) {
        return {0, errors::make("invalid archive file size or position")};
    }

    i64 base = 0;
    if (origin == SeekOrigin::Current) {
        base = static_cast<i64>(position);
    }
    else if (origin == SeekOrigin::End) {
        base = static_cast<i64>(size);
    }
    else if (origin != SeekOrigin::Begin) {
        return {0, errors::make("invalid seek origin")};
    }
    if (offset < -base || offset > static_cast<i64>(size) - base) {
        return {0, errors::make("seek out of range")};
    }
    return {base + offset, nullptr};
}

std::tuple<const pomdogschemas::ArchiveEntry*, std::unique_ptr<Error>>
findArchiveEntry(const pomdogschemas::Archive& archive, u64 key, std::size_t pakSize) noexcept
{
    const auto* keys = archive.keys();
    const auto* entries = archive.entries();
    if (keys == nullptr || entries == nullptr) {
        return {nullptr, errors::make("archive index has no keys or entries")};
    }

    const auto* result = keys->LookupByKey(key);
    if (result == nullptr) {
        return {nullptr, makeNotFoundError("file not found in archive")};
    }

    const auto index = result->index();
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    if (index >= entries->size()) {
        return {nullptr, errors::make("invalid archive index")};
    }

    // NOTE: ArchiveEntry is a struct stored inline in the vector, so Get() never returns nullptr.
    const auto* entry = entries->Get(index);

    const auto compression = entry->compression();
    if (compression == pomdogschemas::ArchiveCompression_None) {
        if (entry->compressed_size() != entry->uncompressed_size()) {
            return {nullptr, errors::make(pomdog::format(
                                 "uncompressed archive entry stores {} bytes for a {}-byte file",
                                 entry->compressed_size(),
                                 entry->uncompressed_size()))};
        }
    }
    else if (compression == pomdogschemas::ArchiveCompression_ZstdDictionary) {
        const auto* dictionaries = archive.dictionaries();
        const auto dictionaryCount = (dictionaries != nullptr) ? dictionaries->size() : 0;
        const auto dictionaryIndex = static_cast<u32>(entry->dictionary_index());
        if (dictionaryIndex >= dictionaryCount) {
            return {nullptr, errors::make(pomdog::format(
                                 "archive entry uses dictionary {}, but the archive has {} dictionaries",
                                 dictionaryIndex,
                                 dictionaryCount))};
        }
    }
    else if (compression != pomdogschemas::ArchiveCompression_Zstd) {
        return {nullptr, errors::make(pomdog::format(
                             "unsupported archive compression {}",
                             static_cast<u32>(compression)))};
    }

    // NOTE: Compute the end in 64 bits so that a corrupted offset cannot wrap around.
    const auto end = static_cast<u64>(entry->start_offset()) + static_cast<u64>(entry->compressed_size());
    if (end > static_cast<u64>(pakSize)) {
        return {nullptr, errors::make(pomdog::format(
                             "archive entry ends at byte {}, beyond the pak file size {}",
                             end,
                             pakSize))};
    }

    return {entry, nullptr};
}

} // namespace pomdog::vfs::detail
