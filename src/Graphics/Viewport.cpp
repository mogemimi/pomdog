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
Viewport::Viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, float minDepth, float maxDepth)
	: bounds(x, y, width, height)
	, minDepth(minDepth)
	, maxDepth(maxDepth)
{}
//-----------------------------------------------------------------------
void Viewport::Width(std::int32_t width)
{
	bounds.width = width;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::Width() const
{
	return bounds.width;
}
//-----------------------------------------------------------------------
void Viewport::Height(std::int32_t height)
{
	bounds.height = height;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::Height() const
{
	return bounds.height;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::TopLeftX() const
{
	return bounds.x;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::TopLeftY() const
{
	return bounds.y;
}
//-----------------------------------------------------------------------
float Viewport::AspectRatio() const
{
	if (bounds.height != 0) {
		return static_cast<float>(bounds.width) / static_cast<float>(bounds.height);
	}
	return 0.0f;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
