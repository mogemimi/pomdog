// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {

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
    SurfaceFormat Format = SurfaceFormat::R8_UNorm;
};

} // namespace Pomdog
