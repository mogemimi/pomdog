// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {

RenderTarget2D::RenderTarget2D(
    GraphicsDevice & graphicsDevice,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn)
    : RenderTarget2D(
        graphicsDevice,
        pixelWidthIn,
        pixelHeightIn,
        false,
        SurfaceFormat::R8G8B8A8_UNorm,
        DepthFormat::None)
{
}

RenderTarget2D::RenderTarget2D(
    GraphicsDevice & graphicsDevice,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    bool generateMipmap,
    SurfaceFormat formatIn,
    DepthFormat depthStencilFormatIn)
    : pixelWidth(pixelWidthIn)
    , pixelHeight(pixelHeightIn)
    , levelCount(generateMipmap ? Detail::TextureHelper::ComputeMipmapLevelCount(pixelWidthIn, pixelHeightIn): 1)
    , format(formatIn)
    , depthStencilFormat(depthStencilFormatIn)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    auto nativeDevice = graphicsDevice.GetNativeGraphicsDevice();
    POMDOG_ASSERT(nativeDevice != nullptr);

    ///@todo MSAA is not implemented yet
    constexpr int multiSampleCount = 1;

    nativeRenderTarget2D = nativeDevice->CreateRenderTarget2D(
        pixelWidth, pixelHeight, levelCount, format, depthStencilFormat,
        multiSampleCount);
}

RenderTarget2D::RenderTarget2D(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn)
    : RenderTarget2D(
        *graphicsDevice,
        pixelWidthIn,
        pixelHeightIn,
        false,
        SurfaceFormat::R8G8B8A8_UNorm,
        DepthFormat::None)
{
}

RenderTarget2D::RenderTarget2D(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    bool generateMipmap,
    SurfaceFormat formatIn,
    DepthFormat depthStencilFormatIn)
    : RenderTarget2D(
        *graphicsDevice,
        pixelWidthIn,
        pixelHeightIn,
        generateMipmap,
        formatIn,
        depthStencilFormatIn)
{
}

RenderTarget2D::~RenderTarget2D() = default;

std::int32_t RenderTarget2D::GetWidth() const
{
    return pixelWidth;
}

std::int32_t RenderTarget2D::GetHeight() const
{
    return pixelHeight;
}

std::int32_t RenderTarget2D::GetLevelCount() const
{
    return levelCount;
}

SurfaceFormat RenderTarget2D::GetFormat() const
{
    return format;
}

DepthFormat RenderTarget2D::GetDepthStencilFormat() const
{
    return depthStencilFormat;
}

Rectangle RenderTarget2D::GetBounds() const
{
    return Rectangle{0, 0, pixelWidth, pixelHeight};
}

Detail::NativeRenderTarget2D* RenderTarget2D::GetNativeRenderTarget2D()
{
    return nativeRenderTarget2D.get();
}

} // namespace Pomdog
