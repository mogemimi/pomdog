//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/Texture2D.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
Texture2D::Texture2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint32_t pixelWidthIn, std::uint32_t pixelHeightIn)
	: nativeTexture2D(graphicsDevice->NativeGraphicsDevice()->CreateTexture2D(
		pixelWidthIn, pixelHeightIn, 0, SurfaceFormat::R8G8B8A8_UNorm))
	, pixelWidth(pixelWidthIn)
	, pixelHeight(pixelHeightIn)
	, levelCount(0)
	, format(SurfaceFormat::R8G8B8A8_UNorm)
{
}
//-----------------------------------------------------------------------
Texture2D::~Texture2D() = default;
//-----------------------------------------------------------------------
std::uint32_t Texture2D::Width() const
{
	return pixelWidth;
}
//-----------------------------------------------------------------------
std::uint32_t Texture2D::Height() const
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
}// namespace Pomdog
