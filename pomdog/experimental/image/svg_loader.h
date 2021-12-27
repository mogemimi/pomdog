// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/content/image/image_buffer.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Texture2D;
class GraphicsDevice;
} // namespace pomdog

namespace pomdog::SVG {

/// Reads a SVG image from data (.svg).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageBuffer, std::unique_ptr<Error>>
Decode(
    const std::uint8_t* data,
    std::size_t size,
    int width,
    int height);

/// Reads a SVG image from file (.svg).
[[nodiscard]] POMDOG_EXPORT std::tuple<ImageBuffer, std::unique_ptr<Error>>
DecodeFile(
    const std::string& filePath,
    int width,
    int height);

/// Creates a texture from SVG file (.svg).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
LoadTexture(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::string& filePath,
    int width,
    int height);

} // namespace pomdog::SVG
