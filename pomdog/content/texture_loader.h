// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class Texture2D;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// Loads a Texture2D from a file using the virtual file system.
/// Supports PNG, DDS, and PNM formats.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTexture2D(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::string_view filePath) noexcept;

/// Loads a Texture2D from a hash-keyed file in a packaged archive.
/// `mountVolumeHashKey` is the xxHash-64 of the mount point's virtual path.
/// `fileHashKey` is the xxHash-64 of the file path relative to the mount point.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTexture2D(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept;

} // namespace pomdog
