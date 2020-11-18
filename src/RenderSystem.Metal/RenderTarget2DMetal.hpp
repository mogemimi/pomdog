// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#import <Metal/MTLTexture.h>

namespace Pomdog::Detail::Metal {

class RenderTarget2DMetal final : public RenderTarget2D {
public:
    RenderTarget2DMetal(
        id<MTLDevice> device,
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount);

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    SurfaceFormat GetFormat() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    DepthFormat GetDepthStencilFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    /// Gets the pointer of the native texture resource.
    id<MTLTexture> GetTexture() const noexcept;

    /// Gets the pointer of the depth stencil texture.
    id<MTLTexture> GetDepthStencilTexture() const noexcept;

private:
    id<MTLTexture> texture = nullptr;
    id<MTLTexture> depthStencilTexture = nullptr;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    std::int32_t levelCount = 0;
    SurfaceFormat format = SurfaceFormat::A8_UNorm;
    DepthFormat depthStencilFormat = DepthFormat::None;
    bool multiSampleEnabled = false;
};

} // namespace Pomdog::Detail::Metal
