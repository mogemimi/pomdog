// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/view_buffer.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <span>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdogschemas {
enum ArchiveCompression : pomdog::u8;
struct Archive;
} // namespace pomdogschemas

namespace pomdog::vfs::detail {

/// ArchiveDictionary is a decoding dictionary for the archive entries that are
/// compressed with it. The files of an archive share it, so it stays in memory
/// while the archive volume or one of its files uses it.
class ArchiveDictionary;

/// Creates a decoding dictionary from `data`, which is in the Zstandard dictionary format.
[[nodiscard]] std::tuple<std::shared_ptr<const ArchiveDictionary>, std::unique_ptr<Error>>
createArchiveDictionary(std::span<const u8> data) noexcept;

/// Creates the decoding dictionaries of `archive`, in the order of its `dictionaries`.
[[nodiscard]] std::tuple<std::vector<std::shared_ptr<const ArchiveDictionary>>, std::unique_ptr<Error>>
createArchiveDictionaries(const pomdogschemas::Archive& archive) noexcept;

/// ArchiveEntryReader reads the bytes that a .pak file stores for one archive entry.
class ArchiveEntryReader {
public:
    ArchiveEntryReader() noexcept;
    virtual ~ArchiveEntryReader() noexcept;

    ArchiveEntryReader(const ArchiveEntryReader&) = delete;
    ArchiveEntryReader& operator=(const ArchiveEntryReader&) = delete;
    ArchiveEntryReader(ArchiveEntryReader&&) = delete;
    ArchiveEntryReader& operator=(ArchiveEntryReader&&) = delete;

    /// Returns stored bytes of the entry from `offset`, which is relative to the
    /// start of the entry. The span may hold only part of the remaining bytes. It
    /// is empty when `offset` is at the end of the entry, and stays valid until
    /// the next call.
    [[nodiscard]] virtual std::tuple<std::span<const u8>, std::unique_ptr<Error>>
    read(std::size_t offset) noexcept = 0;
};

/// CompressedArchiveFile is a read-only File that restores a compressed archive entry.
///
/// The file decodes the entry on the first nonempty read() or view() call. A read
/// of the whole nonempty entry from the beginning decodes directly into the
/// caller's buffer, so the file keeps no copy of the content. Other nonempty
/// reads and view() decode into a buffer that the file keeps until it is closed.
class CompressedArchiveFile final : public File {
private:
    std::unique_ptr<ArchiveEntryReader> reader_;
    std::shared_ptr<const ArchiveDictionary> dictionary_;
    ViewBuffer content_;
    std::size_t size_ = 0;
    std::size_t position_ = 0;
    pomdogschemas::ArchiveCompression compression_;
    bool closed_ = false;

public:
    /// Creates a file that restores `uncompressedSize` bytes from the stored bytes
    /// that `reader` supplies, which are compressed with `compression`. An entry
    /// compressed with a dictionary decodes with `dictionary`, which is nullptr
    /// for the other methods.
    CompressedArchiveFile(
        std::unique_ptr<ArchiveEntryReader> reader,
        pomdogschemas::ArchiveCompression compression,
        std::shared_ptr<const ArchiveDictionary> dictionary,
        std::size_t uncompressedSize) noexcept;

    CompressedArchiveFile(const CompressedArchiveFile&) = delete;
    CompressedArchiveFile& operator=(const CompressedArchiveFile&) = delete;
    CompressedArchiveFile(CompressedArchiveFile&&) = delete;
    CompressedArchiveFile& operator=(CompressedArchiveFile&&) = delete;

    ~CompressedArchiveFile() noexcept override;

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    read(std::span<u8> buffer) noexcept override;

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    write(std::span<const u8> buffer) noexcept override;

    [[nodiscard]] std::tuple<i64, std::unique_ptr<Error>>
    seek(i64 offset, SeekOrigin origin) noexcept override;

    [[nodiscard]] std::unique_ptr<Error>
    close() noexcept override;

    [[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
    stat() noexcept override;

    [[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
    getSize() noexcept override;

    [[nodiscard]] std::tuple<std::span<const u8>, std::unique_ptr<Error>>
    view() noexcept override;

private:
    /// Decodes the whole entry into `dst`, whose size is the uncompressed size.
    [[nodiscard]] std::unique_ptr<Error>
    decode(std::span<u8> dst) noexcept;

    /// Decodes the whole entry into the buffer that the file keeps.
    [[nodiscard]] std::unique_ptr<Error>
    decodeContent() noexcept;
};

} // namespace pomdog::vfs::detail
