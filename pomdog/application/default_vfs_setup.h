// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog {

/// Configuration for the default VFS setup helper.
struct POMDOG_EXPORT DefaultVFSSetupConfig final {
    /// Override for the path to the assets directory (overlay mount).
    /// If empty, no overlay directory is mounted.
    /// Typically set from a `--assets-dir` CLI argument.
    std::string assetsDir;

    /// Override for the path to the archive file (without extension).
    /// If empty, defaults to `<resource_dir>/content.idx`.
    /// Typically set from a `--archive-file` CLI argument.
    std::string archiveFile;

    /// The VFS mount point for assets. Defaults to "/assets".
    std::string mountPoint = "/assets";
};

/// Creates a VFS context and mounts archive/overlay directories based
/// on the given config.
///
/// 1. Creates a new VFS context.
/// 2. If `config.archiveFile` is empty, defaults to `<resource_dir>/content.idx`.
/// 3. Opens the archive (.idx + .pak) and mounts it read-only at `config.mountPoint`.
/// 4. If `config.assetsDir` is non-empty, mounts it as an overlay at `config.mountPoint`.
///
/// @param config The VFS setup configuration.
/// @return A tuple containing the file system context and an error (nullptr on success).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<vfs::FileSystemContext>, std::unique_ptr<Error>>
setupDefaultVFS(const DefaultVFSSetupConfig& config) noexcept;

} // namespace pomdog
