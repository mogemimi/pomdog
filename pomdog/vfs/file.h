// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/enum_cast.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <span>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {

/// FilePermissions represents the file permission flags.
enum class FilePermissions : u8 {
    None = 0,
    Read = (1 << 0),
    Write = (1 << 1),
    ReadWrite = Read | Write,
};

[[nodiscard]] inline constexpr FilePermissions
operator|(FilePermissions a, FilePermissions b) noexcept
{
    return static_cast<FilePermissions>(to_underlying(a) | to_underlying(b));
}

[[nodiscard]] inline constexpr FilePermissions
operator&(FilePermissions a, FilePermissions b) noexcept
{
    return static_cast<FilePermissions>(to_underlying(a) & to_underlying(b));
}

/// FileMode describes how a file should be opened.
struct FileMode final {
    bool read : 1 = false;
    bool write : 1 = false;
    bool create : 1 = false;
    bool append : 1 = false;
    bool truncate : 1 = false;
};

/// SeekOrigin specifies the reference point for seek operations.
enum class SeekOrigin : u8 {
    /// Seek relative to the beginning of the file.
    Begin = 0,

    /// Seek relative to the current file position.
    Current = 1,

    /// Seek relative to the end of the file.
    End = 2,
};

/// FileInfo contains basic information about a file.
class POMDOG_EXPORT FileInfo final {
public:
    std::size_t size = 0;
    bool isDirectory = false;
};

/// File is the base class for all file implementations in the VFS.
class POMDOG_EXPORT File {
public:
    File() noexcept;

    virtual ~File() noexcept;

    File(const File&) = delete;
    File& operator=(const File&) = delete;
    File(File&&) = delete;
    File& operator=(File&&) = delete;

    /// Reads up to buf_size bytes into buf. Returns the number of bytes read, or an error.
    [[nodiscard]] virtual std::tuple<std::size_t, std::unique_ptr<Error>>
    read(std::span<u8> buffer) noexcept = 0;

    /// Writes buf_size bytes from buf. Returns the number of bytes written, or an error.
    [[nodiscard]] virtual std::tuple<std::size_t, std::unique_ptr<Error>>
    write(std::span<const u8> buffer) noexcept = 0;

    /// Seeks to a position in the file. Returns the new absolute position, or an error.
    /// @param offset The byte offset relative to origin.
    /// @param origin The reference point for the offset.
    [[nodiscard]] virtual std::tuple<i64, std::unique_ptr<Error>>
    seek(i64 offset, SeekOrigin origin) noexcept = 0;

    /// Closes the file.
    [[nodiscard]] virtual std::unique_ptr<Error>
    close() noexcept = 0;

    /// Returns file information.
    [[nodiscard]] virtual std::tuple<FileInfo, std::unique_ptr<Error>>
    stat() noexcept = 0;

    /// Returns the size of the file in bytes.
    [[nodiscard]] virtual std::tuple<std::size_t, std::unique_ptr<Error>>
    getSize() noexcept = 0;
};

} // namespace pomdog::vfs
