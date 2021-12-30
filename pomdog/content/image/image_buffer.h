// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct POMDOG_EXPORT ImageBuffer final {
    /// Raw pixel data.
    std::vector<std::uint8_t> RawData;

    /// Pointer to pixel data array.
    const std::uint8_t* PixelData = nullptr;

    /// Size of byte array of pixel data.
    std::size_t ByteLength = 0;

    /// Width of the image in pixels.
    std::int32_t Width = 1;

    /// Height of the image in pixels.
    std::int32_t Height = 1;

    /// The number of mipmap levels in a multilevel texture.
    std::int32_t MipmapCount = 0;

    /// Surface format of the image.
    PixelFormat Format = PixelFormat::R8_UNorm;
};

} // namespace pomdog
