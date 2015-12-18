// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_RENDERTARGET2D_65AA5E90_HPP
#define POMDOG_RENDERTARGET2D_65AA5E90_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Texture.hpp"
#include "DepthFormat.hpp"
#include "SurfaceFormat.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT RenderTarget2D : public Texture {
public:
    RenderTarget2D() = delete;
    RenderTarget2D(RenderTarget2D const&) = delete;
    RenderTarget2D(RenderTarget2D &&) = default;

    RenderTarget2D(GraphicsDevice & graphicsDevice,
        std::int32_t width, std::int32_t height);

    RenderTarget2D(GraphicsDevice & graphicsDevice,
        std::int32_t width, std::int32_t height, bool generateMipmap,
        SurfaceFormat format, DepthFormat depthStencilFormat);

    RenderTarget2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::int32_t width, std::int32_t height);

    RenderTarget2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::int32_t width, std::int32_t height, bool generateMipmap,
        SurfaceFormat format, DepthFormat depthStencilFormat);

    ~RenderTarget2D();

    RenderTarget2D & operator=(RenderTarget2D const&) = delete;
    RenderTarget2D & operator=(RenderTarget2D &&) = default;

    std::int32_t Width() const;

    std::int32_t Height() const;

    std::int32_t LevelCount() const;

    SurfaceFormat Format() const;

    DepthFormat DepthStencilFormat() const;

    Rectangle Bounds() const;

public:
    Detail::NativeRenderTarget2D* NativeRenderTarget2D();

private:
    std::unique_ptr<Detail::NativeRenderTarget2D> nativeRenderTarget2D;
    std::int32_t pixelWidth;
    std::int32_t pixelHeight;
    std::int32_t levelCount;
    SurfaceFormat format;
    DepthFormat depthStencilFormat;
};

} // namespace Pomdog

#endif // POMDOG_RENDERTARGET2D_65AA5E90_HPP
