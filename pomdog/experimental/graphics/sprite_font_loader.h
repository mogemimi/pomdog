// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class SpriteFont;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class Texture2D;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

/// Loads a SpriteFont from a .fnt file via VFS.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
loadSpriteFont(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& filePath);

} // namespace pomdog
