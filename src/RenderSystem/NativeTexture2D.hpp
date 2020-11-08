// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog::Detail {

class NativeTexture2D {
public:
    NativeTexture2D() = default;
    NativeTexture2D(const NativeTexture2D&) = delete;
    NativeTexture2D& operator=(const NativeTexture2D&) = delete;

    virtual ~NativeTexture2D() = default;

    virtual void SetData(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        const void* pixelData) = 0;
};

} // namespace Pomdog::Detail
