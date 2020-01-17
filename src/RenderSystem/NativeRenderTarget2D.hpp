// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>

namespace Pomdog::Detail {

class NativeRenderTarget2D {
public:
    NativeRenderTarget2D() = default;
    NativeRenderTarget2D(const NativeRenderTarget2D&) = delete;
    NativeRenderTarget2D& operator=(const NativeRenderTarget2D&) = delete;

    virtual ~NativeRenderTarget2D() = default;

    virtual void GetData(
        void* result,
        std::size_t offsetInBytes,
        std::size_t sizeInBytes,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format) const = 0;
};

} // namespace Pomdog::Detail
