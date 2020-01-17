// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog::Detail {

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

} // namespace Pomdog::Detail
