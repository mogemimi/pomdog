// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Texture2DMetal.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

namespace Pomdog::Detail::Metal {

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

    void GetData(
        void* result,
        std::size_t offsetInBytes,
        std::size_t sizeInBytes,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format) const override;

    id<MTLTexture> GetTexture() const noexcept;

    id<MTLTexture> GetDepthStencilTexture() const noexcept;

private:
    id<MTLTexture> texture;
    id<MTLTexture> depthStencilTexture;
    //bool multiSampleEnabled;
};

} // namespace Pomdog::Detail::Metal
