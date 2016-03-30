// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeTexture2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

class Texture2DMetal final : public NativeTexture2D {
public:
    Texture2DMetal(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format);

    void SetData(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        void const* pixelData) override;

    id<MTLTexture> GetTexture() const noexcept;

private:
    id<MTLTexture> texture;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
