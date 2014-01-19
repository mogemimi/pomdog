//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/Texture2D.hpp>
#include <Pomdog/Utility/Exception.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
Texture2D::~Texture2D()
{
}
//-----------------------------------------------------------------------
std::uint32_t Texture2D::GetWidth() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "Texture2D::GetWidth");
}
//-----------------------------------------------------------------------
std::uint32_t Texture2D::GetHeight() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "Texture2D::GetHeight");
}
//-----------------------------------------------------------------------
SurfaceFormat Texture2D::GetFormat() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "Texture2D::GetFormat");
}
//-----------------------------------------------------------------------
std::uint32_t Texture2D::GetLevelCount() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error,
		"Not implemented", "Texture2D::GetLevelCount");
}
//-----------------------------------------------------------------------
}// namespace Pomdog
