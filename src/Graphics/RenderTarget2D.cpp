//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "RenderTarget2D.hpp"
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
RenderTarget2D::RenderTarget2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint32_t pixelWidthIn, std::uint32_t pixelHeightIn)
	: RenderTarget2D(graphicsDevice, pixelWidthIn, pixelHeightIn,
		1, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None)
{
}
//-----------------------------------------------------------------------
RenderTarget2D::RenderTarget2D(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::uint32_t pixelWidthIn, std::uint32_t pixelHeightIn, std::uint32_t levelCountIn,
	SurfaceFormat formatIn, DepthFormat depthStencilFormatIn)
	: nativeRenderTarget2D(graphicsDevice->NativeGraphicsDevice()->CreateRenderTarget2D(
		pixelWidthIn, pixelHeightIn, levelCountIn, formatIn, depthStencilFormatIn))
	, pixelWidth(pixelWidthIn)
	, pixelHeight(pixelHeightIn)
	, levelCount(levelCountIn)
	, format(formatIn)
	, depthStencilFormat(depthStencilFormatIn)
{
}
//-----------------------------------------------------------------------
RenderTarget2D::~RenderTarget2D() = default;
//-----------------------------------------------------------------------
std::uint32_t RenderTarget2D::Width() const
{
	return pixelWidth;
}
//-----------------------------------------------------------------------
std::uint32_t RenderTarget2D::Height() const
{
	return pixelHeight;
}
//-----------------------------------------------------------------------
std::uint32_t RenderTarget2D::LevelCount() const
{
	return levelCount;
}
//-----------------------------------------------------------------------
SurfaceFormat RenderTarget2D::Format() const
{
	return format;
}
//-----------------------------------------------------------------------
DepthFormat RenderTarget2D::DepthStencilFormat() const
{
	return depthStencilFormat;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeRenderTarget2D* RenderTarget2D::NativeRenderTarget2D()
{
	return nativeRenderTarget2D.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
