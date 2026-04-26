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

/// Parses a string into a GraphicsBackend value.
///
/// Accepted strings (case-sensitive):
///   "d3d11"   -> Direct3D11
///   "gl4"     -> OpenGL4
///   "vulkan"  -> Vulkan
///   "metal"   -> Metal
///   "webgl"   -> WebGL
///
/// Returns std::nullopt if the string does not match any known backend.
[[nodiscard]] POMDOG_EXPORT std::optional<gpu::GraphicsBackend>
parseGraphicsBackend(std::string_view name) noexcept;

} // namespace pomdog
