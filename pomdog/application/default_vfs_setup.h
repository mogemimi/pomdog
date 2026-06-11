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

/// DefaultVFSSetupConfig configures setupDefaultVFS().
struct POMDOG_EXPORT DefaultVFSSetupConfig final {
    /// The path to a directory mounted as a read-only overlay on top of the
    /// archive; its files shadow archive entries, which allows iterating on
    /// assets without repacking during development. When empty, no overlay
    /// is mounted. Typically set from an `--assets-dir` command-line argument.
    std::string assetsDir;

    /// The path to the archive index file (.idx); the matching .pak path is
    /// derived by replacing the extension. When empty, defaults to
    /// `content.idx` in the platform resource directory. Typically set from
    /// an `--archive-file` command-line argument.
    std::string archiveFile;

    /// The VFS mount point for assets. Defaults to "/assets".
    std::string mountPoint = "/assets";
};

/// Creates a VFS context with the standard asset layout: the archive named
/// by `config.archiveFile` mounted read-only at `config.mountPoint`, with
/// `config.assetsDir` overlaid on top of it when set.
///
/// The archive is required; a missing or unreadable archive is an error
/// even when `config.assetsDir` is set, so a forgotten archive build fails
/// at startup instead of surfacing later as missing assets.
///
/// This helper is optional and covers only this one common layout. For
/// anything else, such as running without an archive, call `vfs::create()`,
/// `vfs::openArchiveFile()`, and `vfs::mount()` directly.
///
/// Call it from `GameSetup::configure()` and hand the returned context to
/// the Game, since the GameSetup instance is destroyed before the game
/// runs. Returns a null context and an error when any step fails.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<vfs::FileSystemContext>, std::unique_ptr<Error>>
setupDefaultVFS(const DefaultVFSSetupConfig& config) noexcept;

} // namespace pomdog
