// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

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
/// @param fs The file system context used to locate the file.
/// @param graphicsDevice The graphics device for creating the texture.
/// @param filePath The virtual path to the texture file.
/// @return A tuple containing the loaded texture and an error object if an error occurred.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTexture2D(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::string_view filePath) noexcept;

} // namespace pomdog
