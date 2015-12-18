// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_TEXTUREHELPER_FDC1E68E_HPP
#define POMDOG_TEXTUREHELPER_FDC1E68E_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {

struct TextureHelper final {
    static std::int32_t ComputeMipmapLevelCount(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight);

    static std::int32_t ComputeTextureSizeInBytes(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_TEXTUREHELPER_FDC1E68E_HPP
