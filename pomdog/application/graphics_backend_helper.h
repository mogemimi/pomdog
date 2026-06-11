// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class GraphicsBackend : u8;
} // namespace pomdog::gpu

namespace pomdog {

/// Parses a backend name into a GraphicsBackend value, or returns
/// std::nullopt when the name does not match any known backend.
///
/// Accepted names (case-sensitive):
///   "d3d11"   -> Direct3D11
///   "gl4"     -> OpenGL4
///   "vulkan"  -> Vulkan
///   "metal"   -> Metal
///   "webgl"   -> WebGL
///
/// Intended for mapping a command-line argument such as `--graphics=gl4` to
/// `GameHostOptions::graphicsBackend` in `GameSetup::configure()`. Parsing
/// does not check whether the backend is available on the current platform
/// or build; the Bootstrap validates that when the GameHost is created.
[[nodiscard]] POMDOG_EXPORT std::optional<gpu::GraphicsBackend>
parseGraphicsBackend(std::string_view name) noexcept;

} // namespace pomdog
