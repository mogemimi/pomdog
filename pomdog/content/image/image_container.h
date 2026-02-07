// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Represents an image container.
class POMDOG_EXPORT ImageContainer final {
public:
    /// Raw pixel data.
    std::vector<u8> rawData;

    /// Pointer to pixel data array.
    const u8* pixelData = nullptr;

    /// Size of byte array of pixel data.
    std::size_t byteLength = 0;

    /// Width of the image in pixels.
    i32 width = 1;

    /// Height of the image in pixels.
    i32 height = 1;

    /// The number of mipmap levels in a multilevel texture.
    i32 mipmapCount = 0;

    /// Surface format of the image.
    PixelFormat format = PixelFormat::R8_UNorm;
};

} // namespace pomdog
