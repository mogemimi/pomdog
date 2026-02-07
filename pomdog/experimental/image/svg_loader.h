// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class ImageContainer;
} // namespace pomdog

namespace pomdog::gpu {
class Texture2D;
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog {

/// Reads a SVG image from data (.svg).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageContainer, std::unique_ptr<Error>>
decodeSVG(
    std::span<const u8> svgData,
    int width,
    int height);

/// Reads a SVG image from file (.svg).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageContainer, std::unique_ptr<Error>>
decodeSVGFromFile(
    const std::string& filePath,
    int width,
    int height);

/// Creates a texture from SVG file (.svg).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTextureFromSVGFile(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& filePath,
    int width,
    int height);

} // namespace pomdog
