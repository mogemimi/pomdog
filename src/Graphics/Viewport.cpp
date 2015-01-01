//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Graphics/Viewport.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
Viewport::Viewport(Rectangle const& boundsIn)
	: Bounds(boundsIn)
	, MinDepth(0.0f)
	, MaxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height)
	: Bounds(x, y, width, height)
	, MinDepth(0.0f)
	, MaxDepth(1.0f)
{}
//-----------------------------------------------------------------------
Viewport::Viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, float minDepthIn, float maxDepthIn)
	: Bounds(x, y, width, height)
	, MinDepth(minDepthIn)
	, MaxDepth(maxDepthIn)
{}
//-----------------------------------------------------------------------
void Viewport::Width(std::int32_t width)
{
	Bounds.Width = width;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::Width() const
{
	return Bounds.Width;
}
//-----------------------------------------------------------------------
void Viewport::Height(std::int32_t height)
{
	Bounds.Height = height;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::Height() const
{
	return Bounds.Height;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::TopLeftX() const
{
	return Bounds.X;
}
//-----------------------------------------------------------------------
std::int32_t Viewport::TopLeftY() const
{
	return Bounds.Y;
}
//-----------------------------------------------------------------------
float Viewport::AspectRatio() const
{
	POMDOG_ASSERT(Bounds.Height != 0);
	if (Bounds.Height != 0) {
		return static_cast<float>(Bounds.Width) / Bounds.Height;
	}
	return 0.0f;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
