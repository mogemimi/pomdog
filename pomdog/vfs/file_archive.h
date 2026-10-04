// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
class MountVolume;
} // namespace pomdog::vfs

namespace pomdog::vfs {

/// Specifies the I/O method used when opening an archive.
enum class ArchiveIOMethod : u8 {
    /// Use standard file I/O (std::ifstream).
    FileStream,

    /// Use memory-mapped I/O when available on the current platform.
    /// Falls back to FileStream on platforms without mmap support (e.g. Emscripten).
    PreferMmap,
};

/// Opens an archive (.idx + .pak) as a MountVolume.
///
/// The .idx file is a FlatBuffers-serialized index that maps xxHash-64 keys
/// to offsets and sizes in the .pak file. The .pak file contains the stored
/// data of the entries. An entry is stored as-is or as a zstd frame, which the
/// file decodes on the first read. Reading a compressed entry whole from the
/// beginning decodes it directly into the caller's buffer; any other read makes
/// the file keep a decoded copy until it is closed. The volume loads the zstd
/// dictionaries in the .idx file when it opens the archive, and keeps them in
/// memory for the entries that are compressed with them.
/// `idxPath` is the path to the .idx file, and `pakPath` is the path to the .pak file.
/// Returns a MountVolume that can be mounted with vfs::mount(), or an error if the
/// archive could not be opened.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
openArchiveFile(const std::string& idxPath, const std::string& pakPath) noexcept;

/// Opens an archive (.idx + .pak) as a MountVolume using the specified I/O method.
///
/// When `ioMethod` is ArchiveIOMethod::PreferMmap and the platform supports
/// memory-mapped I/O (Windows, Linux, macOS), the archive is opened using mmap.
/// Otherwise, standard file I/O is used.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<MountVolume>, std::unique_ptr<Error>>
openArchiveFile(const std::string& idxPath, const std::string& pakPath, ArchiveIOMethod ioMethod) noexcept;

} // namespace pomdog::vfs
