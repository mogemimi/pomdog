//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Graphics/Texture2D.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <algorithm>

namespace Pomdog {
namespace {
//-----------------------------------------------------------------------
static std::uint16_t ComputeMipmapLevelCount(std::int32_t width, std::int32_t height)
{
	POMDOG_ASSERT(width >= 0);
	POMDOG_ASSERT(height >= 0);

	auto size = std::max(width, height);
	std::uint16_t levelCount = 1;
	
	while (size > 1)
	{
		size = size / 2;
		++levelCount;
	}
	return levelCount;
}

}// unnamed namespace
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
	, levelCount(mipMap ? ComputeMipmapLevelCount(pixelWidth, pixelHeight): 1)
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
std::uint32_t Texture2D::LevelCount() const
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
	POMDOG_ASSERT((pixelWidth * pixelHeight) > 0);
	nativeTexture2D->SetData(pixelWidth, pixelHeight, levelCount, format, pixelData);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeTexture2D* Texture2D::NativeTexture2D()
{
	return nativeTexture2D.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
