// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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

    std::int32_t GetWidth() const;

    std::int32_t GetHeight() const;

    std::int32_t GetLevelCount() const;

    SurfaceFormat GetFormat() const;

    DepthFormat GetDepthStencilFormat() const;

    Rectangle GetBounds() const;

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
