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
	std::uint32_t width, std::uint32_t height)
	: nativeTexture2D(graphicsDevice->GetNativeGraphicsDevice()->CreateTexture2D(
		width, height, 0, SurfaceFormat::R8G8B8A8_UNorm))
{
}
//-----------------------------------------------------------------------
Texture2D::~Texture2D() = default;
//-----------------------------------------------------------------------
std::uint32_t Texture2D::GetWidth() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
std::uint32_t Texture2D::GetHeight() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
SurfaceFormat Texture2D::GetFormat() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
std::uint32_t Texture2D::GetLevelCount() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
}// namespace Pomdog
