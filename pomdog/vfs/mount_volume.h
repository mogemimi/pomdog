// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::vfs {
class File;
class FileInfo;
struct FileMode;
} // namespace pomdog::vfs

namespace pomdog::vfs {

/// MountOptions describes how a volume should be mounted.
struct MountOptions final {
    /// If true, the volume is read-only and cannot be written to.
    bool readOnly : 1 = false;

    /// If true, the volume is treated as an overlay filesystem.
    /// An overlay volume is inserted at the front of the mount list,
    /// so its files take priority over other volumes mounted at the same path.
    bool overlayFS : 1 = false;

    /// If true, the volume represents a network filesystem.
    bool network : 1 = false;

    /// If true, the volume is used for persistent data storage (e.g., save files).
    bool persistentDataStorage : 1 = false;

    /// If true, the volume supports hash-key lookup (e.g., for archive files).
    bool hashKeyLookup : 1 = false;
};

/// MountVolume is the base class for all mount volume implementations.
/// Each mount volume maps a virtual path prefix to a set of files.
class POMDOG_EXPORT MountVolume {
public:
    MountVolume() noexcept;

    virtual ~MountVolume() noexcept;

    MountVolume(const MountVolume&) = delete;
    MountVolume& operator=(const MountVolume&) = delete;
    MountVolume(MountVolume&&) = delete;
    MountVolume& operator=(MountVolume&&) = delete;

    /// Opens a file at the given relative path within this volume.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(const std::string& path, const FileMode& mode) noexcept = 0;

    /// Returns true if a file exists at the given relative path within this volume.
    [[nodiscard]] virtual bool
    exists(const std::string& path) noexcept = 0;

    /// Returns file information for the given relative path within this volume.
    [[nodiscard]] virtual std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(const std::string& path) noexcept = 0;

    /// Returns the physical (OS) path for the given relative path, if applicable.
    [[nodiscard]] virtual std::tuple<std::string, std::unique_ptr<Error>>
    toPhysicalPath(const std::string& path) noexcept = 0;

    /// Opens a file by its xxHash-64 key within this volume.
    /// Default implementation returns an error; override in volumes that support hash-key lookup.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
    open(u64 hashKey, const FileMode& mode) noexcept;

    /// Returns true if a file with the given xxHash-64 key exists in this volume.
    /// Default implementation returns false.
    [[nodiscard]] virtual bool
    exists(u64 hashKey) noexcept;

    /// Returns file information by xxHash-64 key.
    /// Default implementation returns an error; override in volumes that support hash-key lookup.
    [[nodiscard]] virtual std::tuple<FileInfo, std::unique_ptr<Error>>
    stat(u64 hashKey) noexcept;
};

} // namespace pomdog::vfs
