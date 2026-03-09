// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/mount_volume.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::vfs {

/// Creates a new VFS (virtual file system) instance.
/// The returned shared_ptr manages the lifetime of the internal FileSystem object.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<FileSystemContext>, std::unique_ptr<Error>>
create() noexcept;

/// Mounts a physical directory at the given virtual path.
/// @param ctx The VFS context returned by create().
/// @param virtualPath The virtual mount point (e.g., "/data").
/// @param physicalPath The physical directory on the OS filesystem.
/// @param options Mount options (readOnly, overlayFS, etc.).
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
mount(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& virtualPath,
    const std::string& physicalPath,
    const MountOptions& options) noexcept;

/// Mounts an existing MountVolume at the given virtual path.
/// @param ctx The VFS context returned by create().
/// @param virtualPath The virtual mount point (e.g., "/data").
/// @param volume The mount volume to attach.
/// @param options Mount options (readOnly, overlayFS, etc.).
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
mount(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& virtualPath,
    std::shared_ptr<MountVolume>&& volume,
    const MountOptions& options) noexcept;

/// Unmounts the volume at the given virtual path.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
unmount(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& virtualPath) noexcept;

/// Creates a directory at the given virtual path.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
mkdir(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept;

/// Recursively creates directories at the given virtual path.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
mkdirAll(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept;

/// Opens a file with the given mode.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path,
    const FileMode& mode) noexcept;

/// Opens a file for reading.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept;

/// Returns file information for the given virtual path.
[[nodiscard]] POMDOG_EXPORT std::tuple<FileInfo, std::unique_ptr<Error>>
stat(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept;

/// Returns true if a file exists at the given virtual path.
[[nodiscard]] POMDOG_EXPORT bool
exists(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept;

/// Resolves a virtual path to a physical (OS) path.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::string, std::unique_ptr<Error>>
resolve(
    const std::shared_ptr<FileSystemContext>& ctx,
    const std::string& path) noexcept;

/// Opens a file by its xxHash-64 key with the given mode.
/// @param mountVolumeHashKey The xxHash-64 of the mount point's virtual path (e.g. computeStringHash64("/assets")).
/// @param fileHashKey The xxHash-64 of the file path relative to the mount point.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey,
    const FileMode& mode) noexcept;

/// Opens a file by its xxHash-64 key for reading.
/// @param mountVolumeHashKey The xxHash-64 of the mount point's virtual path.
/// @param fileHashKey The xxHash-64 of the file path relative to the mount point.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<File>, std::unique_ptr<Error>>
open(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept;

/// Returns file information by xxHash-64 key.
/// @param mountVolumeHashKey The xxHash-64 of the mount point's virtual path.
/// @param fileHashKey The xxHash-64 of the file path relative to the mount point.
[[nodiscard]] POMDOG_EXPORT std::tuple<FileInfo, std::unique_ptr<Error>>
stat(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept;

/// Returns true if a file with the given xxHash-64 key exists.
/// @param mountVolumeHashKey The xxHash-64 of the mount point's virtual path.
/// @param fileHashKey The xxHash-64 of the file path relative to the mount point.
[[nodiscard]] POMDOG_EXPORT bool
exists(
    const std::shared_ptr<FileSystemContext>& ctx,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept;

} // namespace pomdog::vfs
