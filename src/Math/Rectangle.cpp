//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {

Rectangle::Rectangle(std::int32_t x_, std::int32_t y_, std::int32_t width_, std::int32_t height_)
	: x(x_)
	, y(y_)
	, width(width_)
	, height(height_)
{}
//-----------------------------------------------------------------------
Rectangle::Rectangle(Point2D const& position, std::int32_t width_, std::int32_t height_)
	: x(position.x)
	, y(position.y)
	, width(width_)
	, height(height_)
{}
//-----------------------------------------------------------------------
bool Rectangle::operator==(Rectangle const& other) const
{
	return (
		x == other.x &&
		y == other.y &&
		width  == other.width &&
		height == other.height
	);
}
//-----------------------------------------------------------------------
bool Rectangle::operator!=(Rectangle const& other) const
{
	return (
		x != other.x ||
		y != other.y ||
		width  != other.width ||
		height != other.height
	);
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Bottom() const
{
	return y + height;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Right() const
{
	return x + width;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Top() const
{
	return y;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Left() const
{
	return x;
}
//-----------------------------------------------------------------------
Point2D Rectangle::Center() const
{
	return Point2D(x + width / 2, y + height / 2);
}
//-----------------------------------------------------------------------
Point2D Rectangle::Location() const
{
	return {x, y};
}
//-----------------------------------------------------------------------
void Rectangle::Location(Point2D const& position)
{
	this->x = position.x;
	this->y = position.y;
}
//-----------------------------------------------------------------------
void Rectangle::Offset(std::int32_t offsetX, std::int32_t offsetY)
{
	this->x += offsetX;
	this->y += offsetY;
}
//-----------------------------------------------------------------------
void Rectangle::Offset(Point2D const& offset)
{
	x += offset.x;
	y += offset.y;
}

}// namespace Pomdog
