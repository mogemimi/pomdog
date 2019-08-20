// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/Texture.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT RenderTarget2D final : public Texture {
public:
    RenderTarget2D() = delete;
    RenderTarget2D(const RenderTarget2D&) = delete;
    RenderTarget2D(RenderTarget2D&&) = default;

    RenderTarget2D(
        GraphicsDevice& graphicsDevice,
        std::int32_t width,
        std::int32_t height);

    RenderTarget2D(
        GraphicsDevice& graphicsDevice,
        std::int32_t width,
        std::int32_t height,
        bool generateMipmap,
        SurfaceFormat format,
        DepthFormat depthStencilFormat);

    RenderTarget2D(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::int32_t width,
        std::int32_t height);

    RenderTarget2D(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        std::int32_t width,
        std::int32_t height,
        bool generateMipmap,
        SurfaceFormat format,
        DepthFormat depthStencilFormat);

    ~RenderTarget2D();

    RenderTarget2D& operator=(const RenderTarget2D&) = delete;
    RenderTarget2D& operator=(RenderTarget2D&&) = default;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const;

    /// Gets the mipmap level.
    std::int32_t GetLevelCount() const;

    /// Gets the format of the pixel data in the render target.
    SurfaceFormat GetFormat() const;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    DepthFormat GetDepthStencilFormat() const;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const;

    /// Gets the pointer of the native render target.
    Detail::NativeRenderTarget2D* GetNativeRenderTarget2D();

private:
    std::unique_ptr<Detail::NativeRenderTarget2D> nativeRenderTarget2D;
    std::int32_t pixelWidth;
    std::int32_t pixelHeight;
    std::int32_t levelCount;
    SurfaceFormat format;
    DepthFormat depthStencilFormat;
};

} // namespace Pomdog
