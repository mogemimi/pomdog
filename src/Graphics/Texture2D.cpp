// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/Texture2D.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>

namespace Pomdog {
//-----------------------------------------------------------------------
Texture2D::Texture2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    std::int32_t pixelWidthIn, std::int32_t pixelHeightIn)
    : Texture2D(graphicsDevice, pixelWidthIn, pixelHeightIn, false, SurfaceFormat::R8G8B8A8_UNorm)
{}
//-----------------------------------------------------------------------
Texture2D::Texture2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    std::int32_t pixelWidthIn, std::int32_t pixelHeightIn,
    bool mipMap, SurfaceFormat formatIn)
    : pixelWidth(pixelWidthIn)
    , pixelHeight(pixelHeightIn)
    , levelCount(mipMap ? Detail::TextureHelper::ComputeMipmapLevelCount(pixelWidth, pixelHeight): 1)
    , format(formatIn)
{
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    nativeTexture2D = graphicsDevice->NativeGraphicsDevice()->CreateTexture2D(
        pixelWidthIn, pixelHeightIn, levelCount, formatIn);
}
//-----------------------------------------------------------------------
Texture2D::~Texture2D() = default;
//-----------------------------------------------------------------------
std::int32_t Texture2D::Width() const
{
    return pixelWidth;
}
//-----------------------------------------------------------------------
std::int32_t Texture2D::Height() const
{
    return pixelHeight;
}
//-----------------------------------------------------------------------
std::int32_t Texture2D::LevelCount() const
{
    return levelCount;
}
//-----------------------------------------------------------------------
SurfaceFormat Texture2D::Format() const
{
    return format;
}
//-----------------------------------------------------------------------
void Texture2D::SetData(void const* pixelData)
{
    POMDOG_ASSERT(nativeTexture2D);
    POMDOG_ASSERT(pixelData);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);
    nativeTexture2D->SetData(pixelWidth, pixelHeight, levelCount, format, pixelData);
}
//-----------------------------------------------------------------------
Detail::NativeTexture2D* Texture2D::NativeTexture2D()
{
    return nativeTexture2D.get();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
