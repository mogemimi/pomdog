// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Texture2DMetal.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace Metal {

class RenderTarget2DMetal final : public NativeRenderTarget2D {
public:
    RenderTarget2DMetal(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    id<MTLTexture> GetTexture() const noexcept;

    id<MTLTexture> GetDepthStencilTexture() const noexcept;

private:
    id<MTLTexture> texture;
    id<MTLTexture> depthStencilTexture;
    //bool multiSampleEnabled;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
