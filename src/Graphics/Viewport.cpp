//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/Viewport.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
Viewport::Viewport(Rectangle const& boundsIn)
	: bounds(boundsIn)
	, minDepth(0.0f)
	, maxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height)
	: bounds(x, y, width, height)
	, minDepth(0.0f)
	, maxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, float minDepthIn, float maxDepthIn)
	: bounds(x, y, width, height)
	, minDepth(minDepthIn)
	, maxDepth(maxDepthIn)
{}
//-----------------------------------------------------------------------
void Viewport::Width(std::int32_t width)
{
	bounds.Width = width;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::Width() const
{
	return bounds.Width;
}
//-----------------------------------------------------------------------
void Viewport::Height(std::int32_t height)
{
	bounds.Height = height;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::Height() const
{
	return bounds.Height;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::TopLeftX() const
{
	return bounds.X;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::TopLeftY() const
{
	return bounds.Y;
}
//-----------------------------------------------------------------------
float Viewport::AspectRatio() const
{
	if (bounds.Height != 0) {
		return static_cast<float>(bounds.Width) / static_cast<float>(bounds.Height);
	}
	return 0.0f;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
