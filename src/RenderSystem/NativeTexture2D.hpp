// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {

class NativeTexture2D {
public:
    NativeTexture2D() = default;
    NativeTexture2D(NativeTexture2D const&) = delete;
    NativeTexture2D & operator=(NativeTexture2D const&) = delete;

    virtual ~NativeTexture2D() = default;

    virtual void SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
        std::int32_t levelCount, SurfaceFormat format, void const* pixelData) = 0;
};

} // namespace Detail
} // namespace Pomdog
